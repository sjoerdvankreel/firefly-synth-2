#pragma once

#include <playground_base/base/plug/FBPlugConfig.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>
#include <playground_base/dsp/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/FBHostToPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/FBPlugToHostProcessor.hpp>

struct FBHostInputBlock;

template <class Derived>
class FBFixedBlockProcessor:
public IFBHostProcessor
{
  FBPlugAudioBlock _plugAudioOut;
  FBScalarAddrsBase* const _scalar;
  FBHostToPlugProcessor _hostToPlugProcessor;
  FBPlugToHostProcessor _plugToHostProcessor;

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
_plugAudioOut(),
_hostToPlugProcessor(),
_plugToHostProcessor(),
_scalar(scalar) {}

template <class Derived> 
void FBFixedBlockProcessor<Derived>::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.block)
    *_scalar->block[be.index] = be.normalized;

  FBPlugInputBlock const* plugBlock;
  _hostToPlugProcessor.FromHost(input);
  while ((plugBlock = _hostToPlugProcessor.ToPlug()) != nullptr)
  {
    static_cast<Derived*>(this)->ProcessFixed(*plugBlock, _plugAudioOut);
    _plugToHostProcessor.FromPlug(_plugAudioOut);
  }
  _plugToHostProcessor.ToHost(output);
}