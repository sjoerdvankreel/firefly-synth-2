#pragma once

#include <playground_base/base/shared/FBState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

struct FBFixedOutputBlock
{
  FBStateAddrs state = {};
  FBFixedAudioBlock audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};