#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

class FBVoiceBlockParamState final
{
  friend class FBProcParamState;

  float _value = {};
  std::array<float, FB_MAX_VOICES> _voice = {};

  float Value() const { return _value; }
  void Value(float value) { _value = value; };

public:
  void BeginVoice(int slot) { _voice[slot] = _value; }
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceBlockParamState);
  std::array<float, FB_MAX_VOICES> const& Voice() const { return _voice; }
};