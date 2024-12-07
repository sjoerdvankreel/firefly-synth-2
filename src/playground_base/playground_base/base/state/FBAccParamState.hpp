#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

struct alignas(FB_FIXED_BLOCK_ALIGN) FBAccParamState final
{
  FBFixedCVBlock smoothed = {};
  float modulated = 0.0f;
  FBOnePoleFilter smoother = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
};