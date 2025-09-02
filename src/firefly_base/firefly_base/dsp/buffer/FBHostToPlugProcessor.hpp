#pragma once

#include <firefly_base/dsp/buffer/FBBufferBlock.hpp>

struct FBHostInputBlock;

class FBHostToPlugProcessor final
{
  FBFixedInputBlock _fixed = {};
  FBBufferInputBlock _buffer = {};

public:
  FBHostToPlugProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostToPlugProcessor);

  FBFixedInputBlock* ProcessToPlug();
  void BufferFromHost(FBHostInputBlock const& hostBlock);
};