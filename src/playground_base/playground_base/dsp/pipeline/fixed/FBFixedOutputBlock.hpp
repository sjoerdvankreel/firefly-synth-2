#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <unordered_map>

class FBProcStateContainer;

struct FBFixedOutputBlock final
{
  FBProcStateContainer* procState = {};
  FBSIMDArray2<float, FBFixedBlockSamples, 2> audio = {};
  std::unordered_map<int, float> outputParamsNormalized = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};