#pragma once

#include <playground_base/dsp/buffer/FBBufferBlock.hpp>
#include <playground_base/dsp/buffer/FBFixedInputBlock.hpp>

struct FBHostInputBlock;

class FBHostToPlugProcessor final
{
  FBFixedInputBlock _fixed = {};
  FBBufferInputBlock _buffer = {};

public:
  FBFixedInputBlock* ProcessToPlug();
  void BufferFromHost(FBHostInputBlock const& hostBlock);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToPlugProcessor);
};