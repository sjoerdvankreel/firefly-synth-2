#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/host/FBHostBlock.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostAudioBlock.hpp>

#include <vector>
#include <cstdint>

struct FBHostInputBlock final
{
  float bpm = {};
  FBHostAudioBlock audio = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBBlockAutoEvent> blockAuto = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostInputBlock);
  static inline float constexpr DefaultBPM = 120.0f;
};