#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedVectorBlock.hpp>

class alignas(FBVectorByteCount) FBAccParamState final
{
  friend class FBSmoothProcessor;
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  FBFixedVectorBlock _cv = {};
  float _modulated = {};
  FBOnePoleFilter _smoother = {};

  void Modulate(float value) { _modulated = value; }
  void SmoothNext(int sample) { _cv.Sample(sample, _smoother.Next(_modulated)); }
  void Init(float sampleRate) { _smoother = FBOnePoleFilter(sampleRate, FBParamSmoothSec); }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
  FBFixedVectorBlock const& CV() const { return _cv; }
};