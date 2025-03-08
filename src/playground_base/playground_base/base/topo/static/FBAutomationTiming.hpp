#pragma once

#include <string>

enum class FBAutomationTiming
{
  Never,
  PerSample,
  AtVoiceStart
};

std::string
FBAutomationTimingToString(FBAutomationTiming timing);