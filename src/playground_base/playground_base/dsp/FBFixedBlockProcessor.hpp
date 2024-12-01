#pragma once

#include <playground_base/dsp/fixed/FBFixedConfig.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>
#include <playground_base/dsp/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/FBHostToFixedProcessor.hpp>
#include <playground_base/dsp/pipeline/FBFixedToHostProcessor.hpp>

struct FBHostInputBlock;

template <class Derived>
class FBFixedBlockProcessor:
public IFBHostProcessor
{
  FBFixedAudioBlock _audioOut;
  FBScalarAddrsBase* const _scalar;
  FBHostToFixedProcessor _hostToFixed;
  FBFixedToHostProcessor _fixedToHost;

public:
  FBFixedBlockProcessor(
    FBScalarAddrsBase* scalar,
    int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBlockProcessor);

  void ProcessHost(
    FBHostInputBlock const& input, FBHostAudioBlock& output);
};

template <class Derived>
FBFixedBlockProcessor<Derived>::
FBFixedBlockProcessor(
  FBScalarAddrsBase* scalar,
  int maxHostSampleCount) :
_audioOut(),
_hostToFixed(),
_fixedToHost(),
_scalar(scalar) {}

template <class Derived> 
void FBFixedBlockProcessor<Derived>::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.block)
    *_scalar->block[be.index] = be.normalized;

  FBFixedInputBlock const* fixed;
  _hostToFixed.FromHost(input);
  while ((fixed = _hostToFixed.ToFixed()) != nullptr)
  {
    static_cast<Derived*>(this)->ProcessFixed(*fixed, _audioOut);
    _fixedToHost.FromFixed(_audioOut);
  }
  _fixedToHost.ToHost(output);
}