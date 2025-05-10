#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <unordered_map>

struct FBNoteEvent;
class FBVoiceManager;
class FBProcStateContainer;

struct FBPlugInputBlock final
{
  float bpm = {};
  float sampleRate = {};
  FBVoiceManager* voiceManager = {};
  std::vector<FBNoteEvent>* note = {};
  FBSIMDArray2<float, FBFixedBlockSamples, 2> const* audio = {};

  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};

struct FBPlugOutputBlock final
{
  FBProcStateContainer* procState = {};
  FBSIMDArray2<float, FBFixedBlockSamples, 2> audio = {};
  std::unordered_map<int, float> outputParamsNormalized = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugOutputBlock);
};