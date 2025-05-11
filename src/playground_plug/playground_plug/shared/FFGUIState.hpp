#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsState.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>

struct FFGUIState final
{
  std::array<FFGUISettingsGUIState, 1> guiSettings = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUIState);
};