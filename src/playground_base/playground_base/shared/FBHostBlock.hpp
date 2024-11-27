#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>

#define FB_EVENT_COUNT_GUESS 1024

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
  int index;
  int position;
  float normalized;
};

struct FBBlockParamEvent
{
  int index;
  float normalized;
};

struct FBAccumulatedHostEvents
{
  FBAccumulatedHostEvents();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAccumulatedHostEvents);

  std::vector<FBNoteEvent> note;
  std::vector<FBAccParamEvent> accParam;
};

struct FBHostEvents:
public FBAccumulatedHostEvents
{
  FBHostEvents();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostEvents);

  std::vector<FBBlockParamEvent> blockParam;
};

struct FBFixedInputBlock
{
  FBFixedInputBlock();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedInputBlock);

  FBFixedAudioBlock audio;
  FBAccumulatedHostEvents events;
};

struct FBHostInputBlock
{
  FBHostInputBlock(float* l, float* r, int count);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostInputBlock);

  FBHostEvents events;
  FBHostAudioBlock audio;
};

struct FBAccumulatedInputBlock
{
  FBAccumulatedInputBlock(int maxSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAccumulatedInputBlock);

  FBDynamicAudioBlock audio;
  FBAccumulatedHostEvents events;
};