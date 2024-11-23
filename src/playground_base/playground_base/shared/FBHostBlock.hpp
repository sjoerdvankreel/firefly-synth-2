#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>

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

struct FBAccParamEvent
{
  int tag;
  int position;
  float normalized;
};

struct FBBlockParamEvent
{
  int tag;
  float normalized;
};

struct FBHostEvents
{
  FBHostEvents();
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBHostEvents);

  std::vector<FBNoteEvent> note;
  std::vector<FBAccParamEvent> accParam;
  std::vector<FBBlockParamEvent> blockParam;
};

struct FBFixedInputBlock
{
  FBFixedInputBlock();
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBFixedInputBlock);

  FBHostEvents events;
  FFFixedStereoBlock audio;
};

struct FBHostInputBlock
{
  FBHostInputBlock(float* l, float* r, int count);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBHostInputBlock);

  FBHostEvents events;
  FFRawStereoBlockView audio;
};

struct FBAccumulatedInputBlock
{
  FBAccumulatedInputBlock(int maxSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBAccumulatedInputBlock);

  int sampleCount;
  FBHostEvents events;
  FFDynamicStereoBlock audio;
};

struct FBAccumulatedOutputBlock
{
  FBAccumulatedOutputBlock(int maxSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBAccumulatedOutputBlock);

  int sampleCount;
  FFDynamicStereoBlock audio;
};