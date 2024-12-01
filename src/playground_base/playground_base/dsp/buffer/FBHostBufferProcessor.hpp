#pragma once

#include <playground_base/dsp/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/buffer/FBBufferInputBlock.hpp>

struct FBHostInputBlock;

class FBHostBufferProcessor
{
  FBFixedInputBlock _fixed = {};
  FBBufferInputBlock _buffer = {};

public:
  FBFixedInputBlock const* ProcessToFixed();
  void BufferFromHost(FBHostInputBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostBufferProcessor);
};