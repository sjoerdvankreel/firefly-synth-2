#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>

class alignas(FBSIMDAlign) FBGlobalAccParamState final
{
  friend class FBProcParamState;
  friend class FBSmoothingProcessor;

  float _value = {};
  FBAccParamState _global = {};

  float Value() const { return _value; }
  void Value(float value) { _value = value; };
  FBAccParamState& Global() { return _global; }
  void Modulate(float offset) { _global.Modulate(offset); }
  void SmoothNext(int sample) { _global.SmoothNext(sample, _value); }  
  void InitProcessing(float value) { _global.InitProcessing(value); }
  void SetSmoothingCoeffs(int sampleCount) { _global.SetSmoothingCoeffs(sampleCount); }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGlobalAccParamState);
  FBAccParamState const& Global() const { return _global; }
};