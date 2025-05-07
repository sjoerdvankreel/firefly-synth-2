#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <cassert>

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