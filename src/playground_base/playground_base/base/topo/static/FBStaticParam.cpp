#include <playground_base/base/topo/static/FBStaticParam.hpp>

std::string 
FBStaticParamBase::GetDefaultText(int slot) const
{
  if (defaultTextSelector)
    return defaultTextSelector(slot);
  return defaultText;
}

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
  default: assert(false); return {};
  }
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
  default: assert(false); return *static_cast<FBParamNonRealTime const*>(nullptr);
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
  default: assert(false); return *static_cast<FBItemsParamNonRealTime const*>(nullptr);
  }
}

double
FBStaticParamBase::DefaultNormalizedByText(int slot) const
{
  auto text = GetDefaultText(slot);
  if (text.size() == 0)
    return 0.0;
  return NonRealTime().TextToNormalized(false, text).value();
}

std::string
FBStaticParamBase::NormalizedToTextWithUnit(bool io, double normalized) const
{
  std::string result = NonRealTime().NormalizedToText(io, normalized);
  if (!io && !unit.empty())
    result += " " + unit;
  return result;
}