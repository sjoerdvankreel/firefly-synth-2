#pragma once

#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccModEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccAutoEvent.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <vector>

struct FBFixedInputBlock final
{
  FBFixedAudioBlock audio = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBAccModEvent> accModByParamThenSample = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedInputBlock);
};