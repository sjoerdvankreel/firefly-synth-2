#include <playground_plug/base/FFSplitProcessor.hpp>

FFSplitProcessor::
FFSplitProcessor(int maxHostSampleCount):
_inputSplitter(maxHostSampleCount),
_outputSplitter(maxHostSampleCount) {}

void 
FFSplitProcessor::ProcessHostBlock(
  FFHostInputBlock const& inputBlock,
  FFHostOutputBlock& outputBlock)
{
  _inputSplitter.AccumulateHostBlock(inputBlock);
  FFHostInputBlock const* splittedInput = nullptr;
  while ((splittedInput = _inputSplitter.GetFirstFixedBlock()) != nullptr)
  {
    _inputSplitter.RemoveFirstFixedBlock();
  }
}