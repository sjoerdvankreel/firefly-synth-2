#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <unordered_map>

class FBProcStateContainer;

struct FBFixedOutputBlock final
{
  FBFixedFloatAudioBlock audio = {};
  FBProcStateContainer* procState = {};
  std::unordered_map<int, float> outputParamsNormalized = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};