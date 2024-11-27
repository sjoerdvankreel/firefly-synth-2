#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBInputAccumulator.hpp>
#include <playground_base/dsp/FBOutputAccumulator.hpp>
#include <playground_base/dsp/FBHostBlockProcessor.hpp>

struct FBHostInputBlock;

template <class Derived>
class FBFixedBlockProcessor:
public IFBHostBlockProcessor
{
  FBFixedAudioBlock _fixedOutput;
  FBInputAccumulator _inputAccumulator;
  FBOutputAccumulator _outputAccumulator;
  FBScalarParamAddrsBase* const _scalarAddrs;

public:
  FBFixedBlockProcessor(
    FBScalarParamAddrsBase* scalarAddrs,
    int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBlockProcessor);

  void ProcessHost(
    FBHostInputBlock const& input, FBRawAudioBlockView& output);
};

template <class Derived>
FBFixedBlockProcessor<Derived>::
FBFixedBlockProcessor(
  FBScalarParamAddrsBase* scalarAddrs,
  int maxHostSampleCount) :
_fixedOutput(),
_inputAccumulator(maxHostSampleCount),
_outputAccumulator(maxHostSampleCount),
_scalarAddrs(scalarAddrs) {}

template <class Derived> 
void FBFixedBlockProcessor<Derived>::ProcessHost(
  FBHostInputBlock const& input, FBRawAudioBlockView& output)
{
  for (auto const& be : input.events.blockParam)
    *_scalarAddrs->block[be.index] = be.normalized;

  FBFixedInputBlock const* splitted;
  _inputAccumulator.AccumulateFrom(input);
  while (_inputAccumulator.SplitTo(&splitted))
  {
    static_cast<Derived*>(this)->ProcessFixed(*splitted, _fixedOutput);
    _outputAccumulator.AccumulateFrom(_fixedOutput);
  }
  _outputAccumulator.AggregateTo(output);
}