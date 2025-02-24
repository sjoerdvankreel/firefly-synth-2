#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/env/FFEnvState.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsState.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FFGUIState final
{
  std::array<FFGUISettingsGUIState, 1> guiSettings = {};
  std::array<FFEnvGUIState, FFEnvCount> env = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUIState);
};