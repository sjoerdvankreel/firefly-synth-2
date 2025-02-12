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
  FBFixedFloatAudioBlock const* audio = {};
  std::vector<FBNoteEvent> const* note = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};