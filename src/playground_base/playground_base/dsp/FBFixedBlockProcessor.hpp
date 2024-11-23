#pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBInputSplitter.hpp>
#include <playground_base/dsp/FBOutputAggregator.hpp>

class FBFixedBlockProcessor
{
  FFFixedStereoBlock _fixedOutput;
  FBInputSplitter _inputSplitter;
  FBOutputAggregator _outputAggregator;

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBFixedBlockProcessor);
  FBFixedBlockProcessor(int maxHostSampleCount);

  template <class Processor>
  void Process(
    FBHostInputBlock const& input, FFRawStereoBlockView& output, Processor& processor);
};

template <class Processor>
void 
FBFixedBlockProcessor::Process(
  FBHostInputBlock const& input, FFRawStereoBlockView& output, Processor& processor)
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