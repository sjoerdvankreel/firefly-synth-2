#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <cstdint>
#include <unordered_map>

struct FBNoteEvent;
class FBVoiceManager;
class FBProcStateContainer;

struct FBPlugInputBlock final
{
  float bpm = {};
  float sampleRate = {};
  std::uint64_t projectTimeSamples = {};

  FBVoiceManager* voiceManager = {};
  std::vector<FBNoteEvent>* noteEvents = {};
  FBProcStateContainer* procStateContainer = {};
  FBSArray<float, FBFixedBlockSamples> lastMIDINoteKey = {};
  FBSArray2<float, FBFixedBlockSamples, 2> const* audio = {};

  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugInputBlock);
};

struct FBPlugOutputBlock final
{
  FBProcStateContainer* procStateContainer = {};
  FBSArray2<float, FBFixedBlockSamples, 2> audio = {};
  std::unordered_map<int, float> outputParamsNormalized = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugOutputBlock);
};