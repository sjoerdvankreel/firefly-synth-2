#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticParam.hpp>

FBAutomationTiming
FBStaticParam::AutomationTiming() const
{
  if (acc)
    return FBAutomationTiming::PerSample;
  if (!NonRealTime().IsStepped() && IsVoiceBlock())
    return FBAutomationTiming::AtVoiceStart;
  return FBAutomationTiming::Never;
}

std::string
FBAutomationTimingToString(FBAutomationTiming timing)
{
  switch (timing)
  {
  case FBAutomationTiming::Never: return "Never";
  case FBAutomationTiming::PerSample: return "Per Sample";
  case FBAutomationTiming::AtVoiceStart: return "At Voice Start";
  default: FB_ASSERT(false); return {};
  }
}

std::string
FBStaticParamBase::GetDefaultText(int moduleSlot, int paramSlot) const
{
  if (defaultTextSelector)
    return defaultTextSelector(moduleSlot, paramSlot);
  return defaultText;
}

FBParamNonRealTime const&
FBStaticParamBase::NonRealTime() const
{
  switch (type)
  {
  case FBParamType::Log2: return log2;
  case FBParamType::List: return list;
  case FBParamType::Bars: return bars;
  case FBParamType::Linear: return linear;
  case FBParamType::Boolean: return boolean;
  case FBParamType::Discrete: return discrete;
  case FBParamType::Identity: return identity;
  default: 
    FB_ASSERT(false); 
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
    return *static_cast<FBParamNonRealTime const*>(nullptr);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  }
}

FBItemsParamNonRealTime const&
FBStaticParamBase::ItemsNonRealTime() const
{
  switch (type)
  {
  case FBParamType::List: return list;
  case FBParamType::Bars: return bars;
  case FBParamType::Discrete: return discrete;
  default: 
    FB_ASSERT(false); 
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
    return *static_cast<FBItemsParamNonRealTime const*>(nullptr);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  }
}

double
FBStaticParamBase::DefaultNormalizedByText(int moduleSlot, int paramSlot) const
{
  auto text = GetDefaultText(moduleSlot, paramSlot);
  if (text.size() == 0)
    return 0.0;
  auto result = NonRealTime().TextToNormalized(false, text);
  if (result)
    return result.value();
  FB_LOG_WARN("Failed to parse default text '" + text + "' for param '" + name + "'.");
  return 0.0;
}

std::string
FBStaticParamBase::NormalizedToTextWithUnit(bool io, double normalized) const
{
  std::string result = NonRealTime().NormalizedToText(io, normalized);
  if (!io && !unit.empty())
    result += " " + unit;
  return result;
}