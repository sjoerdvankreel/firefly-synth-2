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
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostEvents);

  std::vector<FBNoteEvent> note;
  std::vector<FBAccParamEvent> accParam;
  std::vector<FBBlockParamEvent> blockParam;
};

struct FBFixedInputBlock
{
  FBFixedInputBlock();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedInputBlock);

  FBHostEvents events;
  FFFixedStereoBlock audio;
};

struct FBHostInputBlock
{
  FBHostInputBlock(float* l, float* r, int count);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostInputBlock);

  FBHostEvents events;
  FBRawStereoBlockView audio;
};

struct FBAccumulatedInputBlock
{
  FBAccumulatedInputBlock(int maxSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAccumulatedInputBlock);

  int sampleCount;
  FBHostEvents events;
  FBDynamicStereoBlock audio;
};

struct FBAccumulatedOutputBlock
{
  FBAccumulatedOutputBlock(int maxSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAccumulatedOutputBlock);

  int sampleCount;
  FBDynamicStereoBlock audio;
};