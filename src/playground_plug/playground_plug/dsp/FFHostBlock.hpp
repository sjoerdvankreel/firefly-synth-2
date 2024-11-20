#pragma once

#include <playground_plug/base/FFConfig.hpp>
#include <playground_plug/base/FFUtility.hpp>

#include <array>
#include <vector>

struct FFNoteId
{
  int id;
  int key;
  int channel;
};

struct FFNoteEvent
{
  bool on;
  float velo;
  int position;
  FFNoteId note;
};

struct FFBlockParamEvent
{
  int tag;
  float normalized;
};

struct FFAccurateParamEvent
{
  int tag;
  int position;
  float normalized;
};

struct FFHostBlock
{
  FF_NOCOPY_NOMOVE(FFHostBlock);
  FFHostBlock(int maxSampleCount);

  std::vector<FFNoteEvent> noteEvents = {};
  std::vector<FFBlockParamEvent> blockParamEvents = {};
  std::vector<FFAccurateParamEvent> accurateParamEvents = {};

  std::array<std::vector<float>, FF_CHANNELS_STEREO> audioIn = {};
  std::array<std::vector<float>, FF_CHANNELS_STEREO> audioOut = {};
};