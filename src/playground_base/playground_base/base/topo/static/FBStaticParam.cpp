#include <playground_base/base/topo/static/FBStaticParam.hpp>

using namespace juce;

FBAutomationType 
FBStaticParam::AutomationType() const
{
  if (acc)
    return FBAutomationType::Modulate;
  if (!FBParamTypeIsStepped(type) && IsVoiceBlock())
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
    automationType = "Linear, "; 
    break;
  case FBParamType::Log2:
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