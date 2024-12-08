#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

class alignas(FB_FIXED_BLOCK_ALIGN) FBGlobalAccParamState final
{
  float _value = {};
  FBAccParamState _global = {};

  friend class FBProcParamState;
  float Value() const { return _value; }
  void Value(float value) { _value = value; };

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGlobalAccParamState);
  FBAccParamState const& Global() const { return _global; }
};