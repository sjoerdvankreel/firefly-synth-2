#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

class FBProcStateContainer;

struct FBFixedOutputBlock final
{
  FBFixedAudioBlock audio = {};
  FBProcStateContainer* state = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};