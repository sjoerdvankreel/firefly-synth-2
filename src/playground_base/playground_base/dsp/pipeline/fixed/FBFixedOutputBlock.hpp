#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <map>

class FBProcStateContainer;

struct FBFixedOutputBlock final
{
  FBFixedFloatAudioBlock audio = {};
  FBProcStateContainer* state = {};
  std::map<int, float> outputParamsNormalized = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};