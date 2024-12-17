#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBNote.hpp>
#include <playground_base/dsp/pipeline/host/FBHostAudioBlock.hpp>

#include <vector>

struct FBHostOutputBlock final
{
  FBHostAudioBlock audio = {};
  std::vector<FBNote> returnedVoices = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostOutputBlock);
};