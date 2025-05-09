#pragma once

#include <playground_base/dsp/buffer/FBBufferBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>

struct FBHostInputBlock;

class FBHostToPlugProcessor final
{
  FBBufferBlock _buffer = {};
  FBFixedInputBlock _fixed = {};

public:
  FBFixedInputBlock* ProcessToPlug();
  void BufferFromHost(FBHostInputBlock const& hostBlock);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToPlugProcessor);
};