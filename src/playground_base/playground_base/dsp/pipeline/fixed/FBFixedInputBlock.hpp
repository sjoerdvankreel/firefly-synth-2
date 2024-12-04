#pragma once

#include <playground_base/dsp/pipeline/shared/FBAccEvent.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>

#include <vector>

struct FBFixedInputBlock final:
public FBPlugInputBlock
{
  std::vector<FBAccEvent> acc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedInputBlock);
};