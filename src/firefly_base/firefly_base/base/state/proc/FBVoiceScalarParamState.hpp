#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

#include <array>

// "Scalar" rather than "block" because used for 2 parameter types:
// 1. True per-voice per-block parameters, 
// where the per-voice value is fixed to the global value at block start for voice lifetime.
// 2. Per-voice-start parameters, on which we allow CLAP modulation. 
// Value is fixed to the global value + a per-voice modulator at VOICE (not block) start for voice lifetime.
class FBVoiceScalarParamState final
{
  friend class FBVoiceManager;
  friend class FBProcParamState;

  float _globalValue = {};
  std::array<float, FBMaxVoices> _voice = {};
  std::array<float, FBMaxVoices> _hostModulation = {};

  void GlobalValue(float value) { _globalValue = value; };
  void ModulateByHost(int slot, float offset) { _hostModulation[slot] = offset; }
  void BeginVoice(int slot) { _voice[slot] = _globalValue + _hostModulation[slot]; }

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceScalarParamState);
  float GlobalValue() const { return _globalValue; }
  std::array<float, FBMaxVoices> const& Voice() const { return _voice; }
};