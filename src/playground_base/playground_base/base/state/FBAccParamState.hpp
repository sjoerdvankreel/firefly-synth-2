#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

class alignas(FB_FIXED_BLOCK_ALIGN) FBAccParamState final
{
  FBFixedCVBlock _cv = {};
  float _modulated = {};
  FBOnePoleFilter _smoother = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAccParamState);
  FBFixedCVBlock const& CV() const { return _cv; }
  FBAccParamState(float sampleRate) : _smoother(sampleRate, FB_PARAM_SMOOTH_SEC) {}
};