#include <playground_base/base/topo/static/FBStaticParam.hpp>

using namespace juce;

FBAutomationType 
FBStaticParam::AutomationType() const
{
  if (acc)
    return FBAutomationType::Modulate;
  if (!NonRealTime().IsStepped() && IsVoiceBlock())
    return FBAutomationType::Automate;
  return FBAutomationType::None;
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
  if (!FBParamTypeIsStepped(type) && IsVoiceBlock())
    return automationType + "At Voice Start";
  return "None";
}