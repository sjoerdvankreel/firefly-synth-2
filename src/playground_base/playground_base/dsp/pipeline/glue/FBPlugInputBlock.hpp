#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>

#include <vector>
#include <cstdint>

struct FBNoteEvent;
class FBVoiceManager;

struct FBPlugInputBlock final
{
  float bpm = {};
  float sampleRate = {};
  FBVoiceManager* voiceManager = {};
  std::vector<FBNoteEvent>* note = {};
  FBFixedFloatAudioArray const* audio = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};