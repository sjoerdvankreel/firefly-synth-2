#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/buffer/FBBufferAudioBlock.hpp>
#include <memory>

class FBHostAudioBlock;
class FBFixedAudioBlock;

class FBFixedBufferProcessor
{
  bool _paddedOnce = false;
  bool _hitFixedBlockSize = false;
  FBBufferAudioBlock _buffer = {};

public:
  void ProcessToHost(FBHostAudioBlock& host);
  void BufferFromFixed(FBFixedAudioBlock const& fixed);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedBufferProcessor);
};