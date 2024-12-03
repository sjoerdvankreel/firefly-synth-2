#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

struct FBBufferInputBlock final
{
  FBBufferAudioBlock audio = {};
  std::vector<FBAccEvent> acc = {};
  std::vector<FBNoteEvent> note = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferInputBlock);
};