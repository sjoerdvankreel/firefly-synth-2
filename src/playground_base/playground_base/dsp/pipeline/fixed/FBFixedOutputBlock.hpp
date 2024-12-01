#pragma once

#include <playground_base/base/plug/FBPlugState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

struct FBFixedOutputBlock
{
  FBStateAddrs state = {};
  FBFixedAudioBlock audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};