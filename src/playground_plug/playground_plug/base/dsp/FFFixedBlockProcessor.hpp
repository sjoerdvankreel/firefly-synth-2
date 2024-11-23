#pragma once

#include <playground_plug/base/shared/FFHostBlock.hpp>
#include <playground_plug/base/shared/FFSignalBlock.hpp>
#include <playground_plug/base/dsp/FFInputSplitter.hpp>
#include <playground_plug/base/dsp/FFOutputAggregator.hpp>

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