#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <cstdint>

struct FBNoteEvent;
class FBVoiceManager;
class FBFixedAudioBlock;

struct FBPlugInputBlock final
{
  FBVoiceManager* voiceManager = {};
  FBFixedAudioBlock const* audio = {};
  std::vector<FBNoteEvent> const* note = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};