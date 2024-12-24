#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedVectorBlock.hpp>

#include <algorithm>

class alignas(sizeof(FBFloatVector)) FBAccParamState final
{
  friend class FBSmoothProcessor;
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  FBFixedVectorBlock _cv = {};
  float _modulation = {};
  FBOnePoleFilter _smoother = {};

  void Modulate(float offset) { _modulation = offset; }
  void SetSmoothingCoeffs(float sampleRate, float durationSecs) { _smoother.SetCoeffs(sampleRate, durationSecs); }
  void SmoothNext(int sample, float automation) { _cv.Sample(sample, _smoother.Next(std::clamp(automation + _modulation, 0.0f, 1.0f))); }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
  FBFixedVectorBlock const& CV() const { return _cv; }
};

inline void 
FBAccParamState::SetSmoothingCoeffs(float sampleRate, float durationSecs) 
{ 
  _smoother.SetCoeffs(sampleRate, durationSecs); 
}