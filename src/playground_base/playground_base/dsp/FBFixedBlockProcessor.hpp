#pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBInputSplitter.hpp>
#include <playground_base/dsp/FBOutputAggregator.hpp>

class FFFixedBlockProcessor
{
  FFFixedStereoBlock _fixedOutput;
  FFInputSplitter _inputSplitter;
  FFOutputAggregator _outputAggregator;

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFFixedBlockProcessor);
  FFFixedBlockProcessor(int maxHostSampleCount);

  template <class Processor>
  void Process(
    FFHostInputBlock const& input, FFRawStereoBlockView& output, Processor& processor);
};

template <class Processor>
void 
FFFixedBlockProcessor::Process(
  FFHostInputBlock const& input, FFRawStereoBlockView& output, Processor& processor)
{
  _inputSplitter.Accumulate(input);
  FFFixedInputBlock const* splitted = nullptr;
  while ((splitted = _inputSplitter.Split()) != nullptr)
  {
    processor.ProcessFixed(*splitted, _fixedOutput);
    _outputAggregator.Accumulate(_fixedOutput);
  }
  _outputAggregator.Aggregate(output);
}