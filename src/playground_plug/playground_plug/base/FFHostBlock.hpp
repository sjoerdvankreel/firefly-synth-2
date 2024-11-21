#pragma once

#include <playground_plug/base/FFUtility.hpp>
#include <playground_plug/base/FFDSPBlock.hpp>
#include <playground_plug/base/FFDSPConfig.hpp>

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

struct FFAccParamEvent
{
  int tag;
  int position;
  float normalized;
};

struct FFBlockParamEvent
{
  int tag;
  float normalized;
};

struct FFHostInputBlock
{
  FFHostInputBlock(int maxSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFHostInputBlock);

  int sampleCount;
  FFDynamicStereoBlock audio;
  std::vector<FFNoteEvent> noteEvents;
  std::vector<FFAccParamEvent> accParamEvents;
  std::vector<FFBlockParamEvent> blockParamEvents;
};