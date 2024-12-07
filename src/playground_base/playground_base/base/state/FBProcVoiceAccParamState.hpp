#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcAccParamState.hpp>

struct FBProcVoiceAccParamState final
{
  float value = 0.0f;
  std::array<FBProcAccParamState, FB_MAX_VOICES> proc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcVoiceAccParamState);
};