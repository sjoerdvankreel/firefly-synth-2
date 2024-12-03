#pragma once

#include <playground_base/dsp/pipeline/shared/FBAccEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <vector>

struct FBFixedInputBlock final
{
  FBFixedAudioBlock audio = {};
  std::vector<FBAccEvent> acc = {};
  std::vector<FBNoteEvent> note = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedInputBlock);
};