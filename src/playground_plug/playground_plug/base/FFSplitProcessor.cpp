#include <playground_plug/base/FFSplitProcessor.hpp>

FFSplitProcessor::
FFSplitProcessor(int maxHostSampleCount):
_audioIn(),
_audioOut(),
_inputSplitter(maxHostSampleCount),
_outputSplitter(maxHostSampleCount) {}

void 
FFSplitProcessor::ProcessHostBlock(
  FFHostInputBlock const& input,
  FFHostOutputBlock& output)
{
  _inputSplitter.AccumulateHostBlock(input);
  FFAccumulatingInputBlock const* splittedInput = nullptr;
  while ((splittedInput = _inputSplitter.GetFirstFixedBlock()) != nullptr)
  {
    _inputSplitter.RemoveFirstFixedBlock();
  }
}