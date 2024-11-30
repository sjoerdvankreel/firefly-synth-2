#pragma once

#include <playground_base/base/plug/FBPlugConfig.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>
#include <playground_base/dsp/shared/FBAccEvent.hpp>
#include <playground_base/dsp/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/plug/FBPlugAudioBlock.hpp>
#include <playground_base/dsp/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/host/FBBlockEvent.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>

#define FB_EVENT_COUNT_GUESS 1024

struct FBPipelineEvents
{
  FBPipelineEvents();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPipelineEvents);

  std::vector<FBNoteEvent> note;
  std::vector<FBAccEvent> acc;
};

struct FBHostEvents:
public FBPipelineEvents
{
  FBHostEvents();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostEvents);

  std::vector<FBBlockEvent> block;
};

struct FBPlugInputBlock
{
  FBPlugAudioBlock audio;
  FBPipelineEvents events;
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
  FBPipelineEvents events = {};
};