#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <vector>

class FBProcStateContainer;

struct FBFixedOutputBlock final
{
  FBFixedFloatAudioBlock audio = {};
  FBProcStateContainer* state = {};
  std::vector<float> outputParamsNormalized = {};
  
  FBFixedOutputBlock(int runtimeParamCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedOutputBlock);
};