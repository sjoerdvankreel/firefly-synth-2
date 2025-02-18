#pragma once

#include <playground_plug/modules/env/FFEnvState.hpp>
#include <playground_plug/modules/master/FFMasterState.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FFGUIState final
{
  std::array<FFMasterGUIState, 1> master = {};
  std::array<FFEnvGUIState, FFEnvCount> env = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUIState);
};