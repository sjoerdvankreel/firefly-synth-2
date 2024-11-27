#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBHostBlockProcessor.hpp>
#include <playground_base/pipeline/FBInputAccumulator.hpp>
#include <playground_base/pipeline/FBOutputAccumulator.hpp>

struct FBHostInputBlock;

template <class Derived>
class FBFixedBlockProcessor:
public IFBHostBlockProcessor
{
  FBPlugAudioBlock _plugAudioOut;
  FBInputAccumulator _inputAccumulator;
  FBOutputAccumulator _outputAccumulator;
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
_inputAccumulator(maxHostSampleCount),
_outputAccumulator(maxHostSampleCount),
_scalarAddrs(scalarAddrs) {}

template <class Derived> 
void FBFixedBlockProcessor<Derived>::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.events.blockParam)
    *_scalarAddrs->block[be.index] = be.normalized;

  FBFixedInputBlock const* splitted;
  _inputAccumulator.AccumulateFrom(input);
  while (_inputAccumulator.SplitTo(&splitted))
  {
    static_cast<Derived*>(this)->ProcessFixed(*splitted, _plugAudioOut);
    _outputAccumulator.AccumulateFrom(_plugAudioOut);
  }
  _outputAccumulator.AggregateTo(output);
}