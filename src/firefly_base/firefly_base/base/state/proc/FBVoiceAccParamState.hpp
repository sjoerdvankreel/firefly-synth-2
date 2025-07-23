#pragma once

#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBAccParamState.hpp>

class alignas(FBSIMDAlign) FBVoiceAccParamState final
{
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBProcParamState;
  friend class FBSmoothingProcessor;

  float _value = {};
  std::array<FBAccParamState, FBMaxVoices> _voice = {};

  void InitProcessing(float value);
  void SetSmoothingCoeffs(int sampleCount);

  float Value() const { return _value; }
  void Value(float value) { _value = value; }
  void InitProcessing(int voice, float value) { _voice[voice].InitProcessing(value); }
  void ModulateByHost(int slot, float offset) { _voice[slot].ModulateByHost(offset); }
  void SmoothNextHostValue(int slot, int sample) { _voice[slot].SmoothNextHostValue(sample, _value); }

public:
  FBVoiceAccParamState(float sampleRate);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceAccParamState);
  std::array<FBAccParamState, FBMaxVoices>& Voice() { return _voice; }
  std::array<FBAccParamState, FBMaxVoices> const& Voice() const { return _voice; }
};

inline void
FBVoiceAccParamState::InitProcessing(float value)
{
  for (int v = 0; v < FBMaxVoices; v++)
    _voice[v].InitProcessing(value);
}

inline void
FBVoiceAccParamState::SetSmoothingCoeffs(int sampleCount)
{
  for (int v = 0; v < FBMaxVoices; v++)
    _voice[v].SetSmoothingCoeffs(sampleCount);
}