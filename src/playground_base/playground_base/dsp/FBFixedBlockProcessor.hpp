#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBInputSplitter.hpp>
#include <playground_base/dsp/FBOutputAggregator.hpp>

struct FBHostInputBlock;

template <class Derived>
class FBFixedBlockProcessor:
public IFBHostBlockProcessor
{
  FBFixedStereoBlock _fixedOutput;
  FBInputSplitter _inputSplitter;
  FBOutputAggregator _outputAggregator;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBlockProcessor);
  FBFixedBlockProcessor(int maxHostSampleCount);
  void ProcessHost(FBHostInputBlock const& input, FBRawStereoBlockView& output);
};

template <class Derived>
FBFixedBlockProcessor<Derived>::
FBFixedBlockProcessor(int maxHostSampleCount) :
_fixedOutput(),
_inputSplitter(maxHostSampleCount),
_outputAggregator(maxHostSampleCount) {}

template <class Derived> void 
FBFixedBlockProcessor<Derived>::ProcessHost(
  FBHostInputBlock const& input, FBRawStereoBlockView& output)
{
  _inputSplitter.Accumulate(input);
  FBFixedInputBlock const* splitted = nullptr;
  while ((splitted = _inputSplitter.Split()) != nullptr)
  {
    static_cast<Derived*>(this)->ProcessFixed(*splitted, _fixedOutput);
    _outputAggregator.Accumulate(_fixedOutput);
  }
  _outputAggregator.Aggregate(output);
}