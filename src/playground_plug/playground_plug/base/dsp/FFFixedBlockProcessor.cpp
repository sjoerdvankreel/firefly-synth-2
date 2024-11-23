#include <playground_plug/base/dsp/FFFixedBlockProcessor.hpp>

FFFixedBlockProcessor::
FFFixedBlockProcessor(int maxHostSampleCount):
_fixedOutput(),
_inputSplitter(maxHostSampleCount),
_outputAggregator(maxHostSampleCount) {}

void 
FFFixedBlockProcessor::Process(
  FFHostInputBlock const& input,
  FFRawStereoBlock& output)
{
  _inputSplitter.Accumulate(input);
  FFFixedInputBlock const* splitted = nullptr;
  while ((splitted = _inputSplitter.Split()) != nullptr)
  {
    ProcessFixed(*splitted, _fixedOutput);
    _outputAggregator.Accumulate(_fixedOutput);
  }
  _outputAggregator.Aggregate(output);
}