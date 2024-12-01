#pragma once

#include <playground_base/dsp/fixed/FBFixedAudioBlock.hpp>

struct FBFixedInputBlock
{
  FBFixedAudioBlock audio;

  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedInputBlock);
};