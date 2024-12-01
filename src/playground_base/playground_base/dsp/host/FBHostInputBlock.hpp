#pragma once

#include <playground_base/dsp/shared/FBAccEvent.hpp>
#include <playground_base/dsp/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/host/FBBlockEvent.hpp>
#include <playground_base/dsp/host/FBHostAudioBlock.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>

struct FBHostInputBlock
{
  FBHostAudioBlock audio = {};
  std::vector<FBAccEvent> acc = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBBlockEvent> block = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostInputBlock);
};