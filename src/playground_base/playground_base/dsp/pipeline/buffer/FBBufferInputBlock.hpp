#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccModEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccAutoEvent.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

#include <vector>

struct FBBufferInputBlock final
{
  FBBufferAudioBlock audio = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBAccModEvent> accModByParamThenSample = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferInputBlock);
};