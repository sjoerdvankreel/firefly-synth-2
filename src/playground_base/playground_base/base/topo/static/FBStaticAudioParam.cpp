#include <playground_base/base/topo/static/FBStaticAudioParam.hpp>

using namespace juce;

FBAutomationType 
FBStaticAudioParam::AutomationType() const
{
  if (acc)
    return FBAutomationType::Modulate;
  if (!FBParamTypeIsStepped(type) && IsVoiceBlock())
    return FBAutomationType::Automate;
  return FBAutomationType::None;
}

std::string
FBStaticAudioParam::AutomationTooltip() const
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