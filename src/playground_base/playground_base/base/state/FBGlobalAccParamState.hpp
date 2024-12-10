#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

class alignas(FBSIMDVectorByteCount) FBGlobalAccParamState final
{
  friend class FBProcParamState;
  friend class FBSmoothProcessor;

  float _value = {};
  FBAccParamState _global = {};

  float Value() const { return _value; }
  void Value(float value) { _value = value; };
  FBAccParamState& Global() { return _global; }
  void Modulate(float value) { _global.Modulate(value); }
  void Init(float sampleRate) { _global.Init(sampleRate); }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGlobalAccParamState);
  FBAccParamState const& Global() const { return _global; }
};