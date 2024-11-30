#pragma once

#include <playground_base/base/shared/FBMixin.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>

struct FBPipelineInputBlock
{
  FBPipelineAudioBlock audio;
  std::vector<FBAccEvent> acc;
  std::vector<FBNoteEvent> note;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPipelineInputBlock);
};