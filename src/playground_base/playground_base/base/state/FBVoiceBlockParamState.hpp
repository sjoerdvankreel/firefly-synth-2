#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <array>

struct FBVoiceBlockParamState final
{
  float* value = {};
  std::array<float*, FB_MAX_VOICES> voice = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBVoiceBlockParamState);
};