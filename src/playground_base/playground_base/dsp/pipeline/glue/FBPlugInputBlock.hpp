#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <cstdint>

struct FBNoteEvent;
class FBVoiceManager;
class FBFixedFloatAudioBlock;

struct FBPlugInputBlock final
{
  float bpm = {};
  float sampleRate = {};
  FBVoiceManager* voiceManager = {};
  std::vector<FBNoteEvent>* note = {};
  FBFixedFloatAudioBlock const* audio = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};