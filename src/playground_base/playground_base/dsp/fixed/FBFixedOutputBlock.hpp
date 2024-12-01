#pragma once

#include <playground_base/base/shared/FBStateAddrs.hpp>
#include <playground_base/dsp/fixed/FBFixedAudioBlock.hpp>

struct FBFixedOutputBlock
{
  FBStateAddrs state = {};
  FBFixedAudioBlock audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};