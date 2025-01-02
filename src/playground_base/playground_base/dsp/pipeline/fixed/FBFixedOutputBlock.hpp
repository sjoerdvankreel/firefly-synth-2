#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

class FBProcStateContainer;

struct FBFixedOutputBlock final
{
  FBFixedFloatAudioBlock audio = {};
  FBProcStateContainer* state = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};