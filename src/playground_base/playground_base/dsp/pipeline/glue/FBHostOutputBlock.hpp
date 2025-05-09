#pragma once

#include <playground_base/dsp/shared/FBNote.hpp>
#include <playground_base/dsp/host/FBHostBlock.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <vector>

struct FBHostOutputBlock final
{
  FBHostAudioBlock audio = {};
  std::vector<FBNote> returnedVoices = {};
  std::vector<FBBlockAutoEvent> outputParams = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostOutputBlock);
};