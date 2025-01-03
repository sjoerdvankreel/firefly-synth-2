#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

class alignas(sizeof(FBFloatVector)) FBVoiceAccParamState final
{
  friend class FBVoiceManager;
  friend class FBProcParamState;
  friend class FBSmoothProcessor;

  float _value = {};
  std::array<FBAccParamState, FBMaxVoices> _voice = {};

  void InitProcessing(float value);
  void SetSmoothingCoeffs(float sampleRate, float durationSecs);

  float Value() const { return _value; }
  void Value(float value) { _value = value; }
  std::array<FBAccParamState, FBMaxVoices>& Voice() { return _voice; }
  void Modulate(int slot, float offset) { _voice[slot].Modulate(offset); }
  void SmoothNext(int slot, int sample) { _voice[slot].SmoothNext(sample, _value); }

public:
  FBVoiceAccParamState(float sampleRate);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceAccParamState);
  std::array<FBAccParamState, FBMaxVoices> const& Voice() const { return _voice; }
};

inline void
FBVoiceAccParamState::InitProcessing(float value)
{
  for (int v = 0; v < FBMaxVoices; v++)
    _voice[v].InitProcessing(value);
}

inline void
FBVoiceAccParamState::SetSmoothingCoeffs(float sampleRate, float durationSecs)
{
  for (int v = 0; v < FBMaxVoices; v++)
    _voice[v].SetSmoothingCoeffs(sampleRate, durationSecs);
}