#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferInputBlock.hpp>

struct FBHostInputBlock;

class FBHostBufferProcessor final
{
  FBFixedInputBlock _fixed = {};
  FBBufferInputBlock _buffer = {};

public:
  FBFixedInputBlock* ProcessToFixed();
  void BufferFromHost(FBHostInputBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostBufferProcessor);
};