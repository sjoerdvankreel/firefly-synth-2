#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

#include <unordered_map>

struct FBGraphProcState final
{
  FBPlugInputBlock input = {};
  FBVoiceManager inputVoiceManager;
  FBProcStateContainer procStateContainer;
  FBFixedFloatAudioBlock inputAudio = {};
  std::vector<FBNoteEvent> inputNotes = {};
  std::unordered_map<int, float> outputParamsNormalized = {};
  FB_NOCOPY_MOVE_NODEFCTOR(FBGraphProcState);
};