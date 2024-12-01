#pragma once

#include <playground_base/dsp/fixed/FBFixedAudioBlock.hpp>

struct FBProcAddrs;
struct FBScalarAddrs;

struct FBFixedOutputBlock
{
  FBProcAddrs* proc = {};
  FBScalarAddrs* scalar = {};
  FBFixedAudioBlock audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};