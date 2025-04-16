#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>

#include <array>
#include <algorithm>

class alignas(FBSIMDAlign) FBAccParamState final
{
  friend class FBVoiceManager;
  friend class FBSmoothingProcessor;
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  FBFixedFloatArray _cv = {};
  float _modulation = {};
  FBOnePoleFilter _smoother = {};

  void InitProcessing(float value);
  void SetSmoothingCoeffs(int sampleCount);
  void SmoothNext(int sample, float automation);
  void Modulate(float offset) { _modulation = offset; }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
  float Last() const;
  FBFixedFloatArray const& CV() const { return _cv; }
};

inline float
FBAccParamState::Last() const
{
  return CV()[FBFixedBlockSamples - 1];
}

inline void
FBAccParamState::InitProcessing(float value)
{
  _cv.Fill(value);
}

inline void 
FBAccParamState::SetSmoothingCoeffs(int sampleCount) 
{ 
  _smoother.SetCoeffs(sampleCount);
}

inline void
FBAccParamState::SmoothNext(int sample, float automation) 
{ 
  _cv[sample] = _smoother.Next(std::clamp(automation + _modulation, 0.0f, 1.0f));
}