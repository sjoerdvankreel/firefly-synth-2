#pragma once

#include <playground_plug/base/FFDSPBlock.hpp>
#include <playground_plug/base/shared/FFUtility.hpp>

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

struct FFHostEvents
{
  FFHostEvents();
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFHostEvents);

  std::vector<FFNoteEvent> note;
  std::vector<FFAccParamEvent> accParam;
  std::vector<FFBlockParamEvent> blockParam;
};

struct FFFixedInputBlock
{
  FFFixedInputBlock();
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFFixedInputBlock);

  FFHostEvents events;
  FFFixedStereoBlock audio;
};

struct FFHostInputBlock
{
  FFHostInputBlock(float* l, float* r, int count);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFHostInputBlock);

  FFHostEvents events;
  FFRawStereoBlock audio;
};

struct FFAccumulatingInputBlock
{
  FFAccumulatingInputBlock(int maxSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFAccumulatingInputBlock);

  int sampleCount;
  FFHostEvents events;
  FFDynamicStereoBlock audio;
};

struct FFAccumulatingOutputBlock
{
  FFAccumulatingOutputBlock(int maxSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFAccumulatingOutputBlock);

  int sampleCount;
  FFDynamicStereoBlock audio;
};