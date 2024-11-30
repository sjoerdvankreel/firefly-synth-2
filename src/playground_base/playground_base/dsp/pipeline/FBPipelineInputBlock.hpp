#pragma once

#include <playground_base/base/shared/FBMixin.hpp>
#include <playground_base/dsp/pipeline/FBPipelineEvents.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>

struct FBPipelineInputBlock
{
  FBPipelineEvents events = {};
  FBPipelineAudioBlock audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPipelineInputBlock);
};