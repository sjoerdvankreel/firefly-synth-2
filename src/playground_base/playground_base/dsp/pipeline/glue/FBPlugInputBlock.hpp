#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <vector>
#include <cstdint>

struct FBNoteEvent;
class FBVoiceManager;

struct FBPlugInputBlock final
{
  float bpm = {};
  float sampleRate = {};
  FBVoiceManager* voiceManager = {};
  std::vector<FBNoteEvent>* note = {};
  FBSIMDArray2<float, FBFixedBlockSamples, 2> const* audio = {};

  FB_NOCOPY_MOVE_DEFCTOR(FBPlugInputBlock);
};