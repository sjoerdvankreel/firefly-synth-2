#include <playground_base/base/topo/static/FBStaticParam.hpp>

using namespace juce;

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
FBStaticParam::AutomationTooltip() const
{
  std::string automationType = "";
  switch (type)
  {
  case FBParamType::Linear: 
  case FBParamType::Discrete: // TODO
    automationType = "Linear, "; 
    break;
  case FBParamType::Log2:
  case FBParamType::DiscreteLog2: // TODO
    automationType = "Logarithmic, ";
    break;
  default:
    break;
  }
  if (acc)
    if (IsVoice())
      return automationType + "Sample Accurate Per Voice";
    else
      return automationType + "Sample Accurate";
  if (!NonRealTime().IsStepped() && IsVoiceBlock())
    return automationType + "At Voice Start";
  return "None";
}