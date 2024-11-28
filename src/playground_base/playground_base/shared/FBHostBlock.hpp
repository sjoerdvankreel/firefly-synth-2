#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>

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

struct FBPipelineHostEvents
{
  FBPipelineHostEvents();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPipelineHostEvents);

  std::vector<FBNoteEvent> note;
  std::vector<FBAccParamEvent> accParam;
};

struct FBHostEvents:
public FBPipelineHostEvents
{
  FBHostEvents();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostEvents);

  std::vector<FBBlockParamEvent> blockParam;
};

struct FBPlugInputBlock
{
  FBPlugAudioBlock audio;
  FBPipelineHostEvents events;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugInputBlock);
};

struct FBHostInputBlock
{
  FBHostInputBlock(float* l, float* r, int count);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostInputBlock);

  FBHostEvents events;
  FBHostAudioBlock audio;
};

struct FBPipelineInputBlock
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPipelineInputBlock);

  FBPipelineAudioBlock audio = {};
  FBPipelineHostEvents events = {};
};