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
FBStaticParamBase::GetDefaultText(int moduleIndex, int moduleSlot, int paramSlot) const
{
  if (defaultTextSelector)
    return defaultTextSelector(moduleIndex, moduleSlot, paramSlot);
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

std::string 
FBStaticParamBase::DebugNameAndId() const
{
  return std::string(name) + " (" + id + ")";
}

std::optional<double>
FBStaticParamBase::TextToPlain(bool io, int moduleIndex, std::string const& text) const
{
  return NonRealTime().TextToPlain(*this, io, moduleIndex, text);
}

std::optional<double>
FBStaticParamBase::TextToNormalized(bool io, int moduleIndex, std::string const& text) const
{
  return NonRealTime().TextToNormalized(*this, io, moduleIndex, text);
}

std::string
FBStaticParamBase::NormalizedToText(bool io, int moduleIndex, double normalized) const
{
  return NonRealTime().NormalizedToText(io, moduleIndex, normalized);
}

std::string
FBStaticParamBase::NormalizedToTextWithUnit(bool io, int moduleIndex, double normalized) const
{
  std::string result = NonRealTime().NormalizedToText(io, moduleIndex, normalized);
  if (!io && !unit.empty())
    result += " " + unit;
  return result;
}

double
FBStaticParamBase::DefaultNormalizedByText(int moduleIndex, int moduleSlot, int paramSlot) const
{
  auto text = GetDefaultText(moduleIndex, moduleSlot, paramSlot);
  FB_ASSERT(text.size());
  auto result = TextToNormalized(false, moduleIndex, text);
#ifndef NDEBUG
  if (result.has_value())
  {
    std::string roundtrip = NormalizedToText(false, moduleIndex, result.value());
    if (roundtrip != text)
    {
      FB_ASSERT(false);
      NormalizedToText(false, moduleIndex, result.value());
    }
  }
#endif
  if (result)
    return result.value();
#ifndef NDEBUG
  FB_ASSERT(false);
  TextToNormalized(false, moduleIndex, text);
#endif
  FB_LOG_WARN("Failed to parse default text.");
  return 0.0;
}