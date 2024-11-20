#pragma once

#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <array>
#include <vector>

struct FBNoteId
{
  int id;
  int key;
  int channel;
};

struct FBNoteEvent
{
  bool on;
  float velo;
  int position;
  FBNoteId note;
};

struct FBBlockParamEvent
{
  int tag;
  float normalized;
};

struct FBAccurateParamEvent
{
  int tag;
  int position;
  float normalized;
};

struct FBHostBlock
{
  FB_NOCOPY_NOMOVE(FBHostBlock);
  FBHostBlock(int maxSampleCount);

  int currentSampleCount = 0;
  std::vector<FBNoteEvent> noteEvents = {};
  std::vector<FBBlockParamEvent> blockParamEvents = {};
  std::vector<FBAccurateParamEvent> accurateParamEvents = {};
  std::array<std::vector<float>, FB_CHANNELS_STEREO> audioIn = {};
  std::array<std::vector<float>, FB_CHANNELS_STEREO> audioOut = {};
};