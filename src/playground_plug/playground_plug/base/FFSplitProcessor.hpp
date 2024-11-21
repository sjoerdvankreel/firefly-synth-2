#pragma once

#include <playground_plug/base/FFHostBlock.hpp>
#include <playground_plug/base/FFInputSplitter.hpp>
#include <playground_plug/base/FFOutputSplitter.hpp>

class FFSplitProcessor
{
  FFInputSplitter _inputSplitter;
  FFOutputSplitter _outputSplitter;

public:
  FFSplitProcessor(int maxHostSampleCount);

  void ProcessHostBlock(
    FFHostInputBlock const& inputBlock, 
    FFHostOutputBlock& outputBlock);
};