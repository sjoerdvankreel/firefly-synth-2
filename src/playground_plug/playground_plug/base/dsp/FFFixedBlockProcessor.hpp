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

protected:
  virtual void ProcessFixed(
    FFFixedInputBlock const& input,
    FFFixedStereoBlock& output) = 0;

public:
  FFFixedBlockProcessor(int maxHostSampleCount);

  void Process(
    FFHostInputBlock const& input, 
    FFRawStereoBlock& output);
};