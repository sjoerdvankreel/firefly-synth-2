#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

struct FBVoiceAccParamState final
{
  float value = 0.0f;
  std::array<FBAccParamState, FB_MAX_VOICES> proc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceAccParamState);
};