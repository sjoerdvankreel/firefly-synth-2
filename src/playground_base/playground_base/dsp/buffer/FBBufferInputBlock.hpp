#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBAccEvent.hpp>
#include <playground_base/dsp/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/buffer/FBBufferAudioBlock.hpp>

struct FBBufferInputBlock
{
  FBBufferAudioBlock audio;
  std::vector<FBAccEvent> acc;
  std::vector<FBNoteEvent> note;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferInputBlock);
};