#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>

#include <array>
#include <algorithm>

class alignas(FBSIMDAlign) FBAccParamState final
{
  friend class FBVoiceManager;
  friend class FBSmoothingProcessor;
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  float _modulation = {};
  FBOnePoleFilter _smoother = {};
  FBSArray<float, FBFixedBlockSamples> _cv = {};

  void Modulate(float offset) { _modulation = offset; }
  void InitProcessing(float value) { _cv.Fill(value); }
  void SetSmoothingCoeffs(int sampleCount) { _smoother.SetCoeffs(sampleCount); }
  
  void SmoothNext(int sample, float automation) 
  { _cv.Set(sample, _smoother.Next(std::clamp(automation + _modulation, 0.0f, 1.0f))); }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
  float Last() const { return CV().Get(FBFixedBlockSamples - 1); }
  FBSArray<float, FBFixedBlockSamples> const& CV() const { return _cv; }
};
