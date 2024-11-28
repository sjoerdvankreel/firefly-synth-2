#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBHostBlockProcessor.hpp>
#include <playground_base/dsp/pipeline/FBHostToPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/FBPlugToHostProcessor.hpp>

struct FBHostInputBlock;

template <class Derived>
class FBFixedBlockProcessor:
public IFBHostBlockProcessor
{
  FBPlugAudioBlock _plugAudioOut;
  FBHostToPlugProcessor _hostToPlugProcessor;
  FBPlugToHostProcessor _plugToHostProcessor;
  FBScalarParamAddrsBase* const _scalarAddrs;

public:
  FBFixedBlockProcessor(
    FBScalarParamAddrsBase* scalarAddrs,
    int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBlockProcessor);

  void ProcessHost(
    FBHostInputBlock const& input, FBHostAudioBlock& output);
};

template <class Derived>
FBFixedBlockProcessor<Derived>::
FBFixedBlockProcessor(
  FBScalarParamAddrsBase* scalarAddrs,
  int maxHostSampleCount) :
_plugAudioOut(),
_hostToPlugProcessor(),
_plugToHostProcessor(),
_scalarAddrs(scalarAddrs) {}

template <class Derived> 
void FBFixedBlockProcessor<Derived>::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.events.blockParam)
    *_scalarAddrs->block[be.index] = be.normalized;

  FBPlugInputBlock const* plugBlock;
  _hostToPlugProcessor.FromHost(input);
  while ((plugBlock = _hostToPlugProcessor.ToPlug()) != nullptr)
  {
    static_cast<Derived*>(this)->ProcessFixed(*plugBlock, _plugAudioOut);
    _plugToHostProcessor.FromPlug(_plugAudioOut);
  }
  _plugToHostProcessor.ToHost(output);
}