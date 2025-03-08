#include <playground_base/base/topo/static/FBAutomationTiming.hpp>
#include <cassert>

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