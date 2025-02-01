#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/glue/FBBlockEvent.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccModEvent.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccAutoEvent.hpp>

#include <vector>
#include <cstdint>

struct FBHostInputBlock final
{
  FBHostAudioBlock audio = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBBlockEvent> block = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostInputBlock);
};