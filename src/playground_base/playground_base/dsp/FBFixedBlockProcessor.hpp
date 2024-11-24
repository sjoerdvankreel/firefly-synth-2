#pragma once

#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBInputSplitter.hpp>
#include <playground_base/dsp/FBOutputAggregator.hpp>

struct FBHostInputBlock;

class FBFixedBlockProcessor
{
  FBFixedStereoBlock _fixedOutput;
  FBInputSplitter _inputSplitter;
  FBOutputAggregator _outputAggregator;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBlockProcessor);
  FBFixedBlockProcessor(int maxHostSampleCount);

  template <class Processor>
  void ProcessHost(
    FBHostInputBlock const& input, FBRawStereoBlockView& output, Processor& processor);
};

template <class Processor>
void 
FBFixedBlockProcessor::ProcessHost(
  FBHostInputBlock const& input, FBRawStereoBlockView& output, Processor& processor)
{
  _inputSplitter.Accumulate(input);
  FBFixedInputBlock const* splitted = nullptr;
  while ((splitted = _inputSplitter.Split()) != nullptr)
  {
    processor.ProcessFixed(*splitted, _fixedOutput);
    _outputAggregator.Accumulate(_fixedOutput);
  }
  _outputAggregator.Aggregate(output);
}