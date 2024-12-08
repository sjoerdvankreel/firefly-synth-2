#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

class alignas(FB_FIXED_BLOCK_ALIGN) FBVoiceAccParamState final
{
  friend class FBProcParamState;

  float _value = {};
  std::array<FBAccParamState, FB_MAX_VOICES> _voice;

  float Value() const { return _value; }
  void Value(float value) { _value = value; };

public:
  FBVoiceAccParamState(float sampleRate);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVoiceAccParamState);
  std::array<FBAccParamState, FB_MAX_VOICES> const& Voice() const { return _voice; }
};