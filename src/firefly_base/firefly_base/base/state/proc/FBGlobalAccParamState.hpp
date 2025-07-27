#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBAccParamState.hpp>

class alignas(FBSIMDAlign) FBGlobalAccParamState final
{
  friend class FBHostProcessor;
  friend class FBProcParamState;
  friend class FBSmoothingProcessor;

  float _value = {};
  FBAccParamState _global = {};

  float Value() const { return _value; }
  void Value(float value) { _value = value; };
  void InitProcessing(float value) { _global.InitProcessing(value); }
  void ModulateByHost(float offset) { _global.ModulateByHost(offset); }
  void SmoothNextHostValue(int sample) { _global.SmoothNextHostValue(sample, _value); }
  void SetSmoothingCoeffs(int sampleCount) { _global.SetSmoothingCoeffs(sampleCount); }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGlobalAccParamState);
  FBAccParamState& Global() { return _global; }
  FBAccParamState const& Global() const { return _global; }
};