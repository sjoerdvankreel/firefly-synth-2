#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <cstdint>

struct FBNoteEvent;
class FBFixedAudioBlock;

struct FBPlugInputBlock final
{
  FBFixedAudioBlock const* audio = {};
  std::vector<FBNoteEvent> const* note = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};