#pragma once

class FBHostProcessor
{
public:
  void ProcessHost(
    FBHostInputBlock const& input, FBHostAudioBlock& output);
};

#include <playground_base/dsp/fixed/FBFixedConfig.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>
#include <playground_base/dsp/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/buffer/FBFixedBufferProcessor.hpp>

struct FBHostInputBlock;

template <class Derived>
class FBFixedBlockProcessor:
public IFBHostProcessor
{
  FBFixedAudioBlock _audioOut;
  FBScalarAddrs* const _scalar;
  FBHostBufferProcessor _hostBuffer;
  FBFixedBufferProcessor _fixedBuffer;

public:
  FBFixedBlockProcessor(
    FBScalarAddrs* scalar,
    int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBlockProcessor);

  void ProcessHost(
    FBHostInputBlock const& input, FBHostAudioBlock& output);
};

template <class Derived>
FBFixedBlockProcessor<Derived>::
FBFixedBlockProcessor(
  FBScalarAddrs* scalar,
  int maxHostSampleCount) :
_audioOut(),
_hostBuffer(),
_fixedBuffer(),
_scalar(scalar) {}

template <class Derived> 
void FBFixedBlockProcessor<Derived>::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.block)
    *_scalar->block[be.index] = be.normalized;

  FBFixedInputBlock const* fixed;
  _hostBuffer.BufferFromHost(input);
  while ((fixed = _hostBuffer.ProcessToFixed()) != nullptr)
  {
    static_cast<Derived*>(this)->ProcessFixed(*fixed, _audioOut);
    _fixedBuffer.BufferFromFixed(_audioOut);
  }
  _fixedBuffer.ProcessToHost(output);
}