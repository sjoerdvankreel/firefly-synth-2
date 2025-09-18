#pragma once

#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleStateVoiceStart.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFVoiceModuleProcessor final
{
  int _portaPitchSamplesTotal = 0;
  int _portaPitchSamplesProcessed = 0;
  float _portaPitchDelta = 0.0f;
  float _portaPitchOffsetCurrent = 0.0f;

  FFVoiceModuleVoiceStartParamState<float> _voiceStartSnapshotNorm = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceModuleProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state, float previousMidiKeyUntuned, bool anyNoteWasOnAlready);
};