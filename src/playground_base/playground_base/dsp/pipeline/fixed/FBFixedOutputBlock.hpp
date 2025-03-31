#pragma once

#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <unordered_map>

class FBProcStateContainer;

struct FBFixedOutputBlock final
{
  FBFixedFloatAudioArray audio = {};
  FBProcStateContainer* procState = {};
  std::unordered_map<int, float> outputParamsNormalized = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};