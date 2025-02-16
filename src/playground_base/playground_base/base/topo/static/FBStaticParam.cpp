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
  if (acc)
    if (IsVoice())
      return "Sample Accurate Per Voice";
    else
      return "Sample Accurate";
  if (!FBParamTypeIsStepped(type) && IsVoiceBlock())
    return "At Voice Start";
  return "None";
}