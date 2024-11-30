#pragma once

#include <playground_base/dsp/shared/FBAccEvent.hpp>
#include <playground_base/dsp/shared/FBNoteEvent.hpp>
#include <playground_base/base/shared/FBObjectLifetime.hpp>

#include <vector>

struct FBPipelineEvents
{
  std::vector<FBAccEvent> acc;
  std::vector<FBNoteEvent> note;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPipelineEvents);
};