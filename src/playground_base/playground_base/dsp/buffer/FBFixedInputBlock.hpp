#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/host/FBHostBlock.hpp>

#include <vector>

struct FBFixedInputBlock final
{
  std::vector<FBNoteEvent> note = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  FBSIMDArray2<float, FBFixedBlockSamples, 2> audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedInputBlock);
};