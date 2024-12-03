#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/host/FBBlockEvent.hpp>
#include <playground_base/dsp/pipeline/host/FBHostAudioBlock.hpp>

#include <vector>

struct FBHostInputBlock final
{
  FBHostAudioBlock audio = {};
  std::vector<FBAccEvent> acc = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBBlockEvent> block = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostInputBlock);
};