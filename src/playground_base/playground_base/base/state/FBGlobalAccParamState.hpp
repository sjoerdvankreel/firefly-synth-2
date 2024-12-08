#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

class alignas(FB_FIXED_BLOCK_ALIGN) FBGlobalAccParamState final
{
  friend class FBProcParamState;

  float _value = {};
  FBAccParamState _global;

  float Value() const { return _value; }
  void Value(float value) { _value = value; };

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGlobalAccParamState);
  FBAccParamState const& Global() const { return _global; }
  FBGlobalAccParamState(float sampleRate) : _global(sampleRate) {}
};