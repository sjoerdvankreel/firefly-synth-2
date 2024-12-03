#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

struct FBProcState;

struct FBFixedOutputBlock
{
  FBProcState* state = {};
  FBFixedAudioBlock audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};