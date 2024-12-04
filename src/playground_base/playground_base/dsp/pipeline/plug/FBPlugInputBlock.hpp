#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <vector>
#include <cstdint>

struct FBPlugInputBlock
{
  FBFixedAudioBlock audio = {};
  std::vector<FBNoteEvent> note = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};