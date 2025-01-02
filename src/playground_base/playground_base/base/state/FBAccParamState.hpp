#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>

#include <array>
#include <algorithm>

class alignas(sizeof(FBFloatVector)) FBAccParamState final
{
  friend class FBSmoothProcessor;
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  std::array<float, FBFixedBlockSamples> _cv = {};
  float _modulation = {};
  FBOnePoleFilter _smoother = {};

  void SmoothNext(int sample, float automation);
  void Modulate(float offset) { _modulation = offset; }
  void SetSmoothingCoeffs(float sampleRate, float durationSecs);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
  FBFloatVector CV(int v) const;
  std::array<float, FBFixedBlockSamples> CV() const { return _cv; }
};

inline FBFloatVector
FBAccParamState::CV(int v) const
{
  return FBFloatVector::load_aligned(_cv.data() + v * FBVectorFloatCount);
}

inline void 
FBAccParamState::SetSmoothingCoeffs(float sampleRate, float durationSecs) 
{ 
  _smoother.SetCoeffs(sampleRate, durationSecs); 
}

inline void
FBAccParamState::SmoothNext(int sample, float automation) 
{ 
  _cv[sample] = _smoother.Next(std::clamp(automation + _modulation, 0.0f, 1.0f));
}