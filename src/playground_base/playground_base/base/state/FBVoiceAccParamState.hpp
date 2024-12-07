#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

struct alignas(FB_FIXED_BLOCK_ALIGN) FBVoiceAccParamState final
{
  std::array<FBAccParamState, FB_MAX_VOICES> proc = {};
  float value = 0.0f;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceAccParamState);
};