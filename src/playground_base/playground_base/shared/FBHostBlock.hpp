#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>

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

struct FBHostEvents
{
  FBHostEvents();
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBHostEvents);

  std::vector<FFNoteEvent> note;
  std::vector<FFAccParamEvent> accParam;
  std::vector<FFBlockParamEvent> blockParam;
};

struct FBFixedInputBlock
{
  FBFixedInputBlock();
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBFixedInputBlock);

  FBHostEvents events;
  FFFixedStereoBlock audio;
};

struct FFHostInputBlock
{
  FFHostInputBlock(float* l, float* r, int count);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFHostInputBlock);

  FBHostEvents events;
  FFRawStereoBlockView audio;
};

struct FFAccumulatedInputBlock
{
  FFAccumulatedInputBlock(int maxSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFAccumulatedInputBlock);

  int sampleCount;
  FBHostEvents events;
  FFDynamicStereoBlock audio;
};

struct FFAccumulatedOutputBlock
{
  FFAccumulatedOutputBlock(int maxSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFAccumulatedOutputBlock);

  int sampleCount;
  FFDynamicStereoBlock audio;
};