#pragma once

#include <playground_base/dsp/shared/FBAccEvent.hpp>
#include <playground_base/dsp/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/plug/FBPlugAudioBlock.hpp>

#include <vector>

struct FBPlugInputBlock
{
  FBPlugAudioBlock audio;
  std::vector<FBAccEvent> acc;
  std::vector<FBNoteEvent> note;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugInputBlock);
};