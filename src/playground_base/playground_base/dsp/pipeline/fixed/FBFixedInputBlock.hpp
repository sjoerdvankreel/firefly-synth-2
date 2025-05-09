#pragma once

#include <playground_base/dsp/host/FBHostBlock.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>

#include <vector>

struct FBFixedInputBlock final
{
  FBFixedFloatAudioArray audio = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedInputBlock);
};