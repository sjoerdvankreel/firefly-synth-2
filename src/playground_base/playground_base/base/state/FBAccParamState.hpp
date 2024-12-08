#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

class alignas(FB_FIXED_BLOCK_ALIGN) FBAccParamState final
{
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  FBFixedCVBlock _cv = {};
  float _modulated = {};
  FBOnePoleFilter _smoother = {};

  void Init(float sampleRate) 
  { _smoother = FBOnePoleFilter(sampleRate, FB_PARAM_SMOOTH_SEC); }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
  FBFixedCVBlock const& CV() const { return _cv; }
};