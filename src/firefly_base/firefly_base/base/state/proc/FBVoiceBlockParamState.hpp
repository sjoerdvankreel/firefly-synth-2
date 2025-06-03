#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>

class FBVoiceBlockParamState final
{
  friend class FBVoiceManager;
  friend class FBProcParamState;

  float _value = {};
  std::array<float, FBMaxVoices> _voice = {};

  float Value() const { return _value; }
  void Value(float value) { _value = value; };
  void BeginVoice(int slot) { _voice[slot] = _value; }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceBlockParamState);
  std::array<float, FBMaxVoices> const& Voice() const { return _voice; }
};