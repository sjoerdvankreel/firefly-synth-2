#pragma once

#include <playground_plug/base/FFDSPBlock.hpp>
#include <playground_plug/base/FFHostBlock.hpp>
#include <playground_plug/base/FFInputSplitter.hpp>
#include <playground_plug/base/FFOutputSplitter.hpp>

class FFSplitProcessor
{
  FFFixedStereoBlock _audioIn;
  FFFixedStereoBlock _audioOut;
  FFInputSplitter _inputSplitter;
  FFOutputSplitter _outputSplitter;

protected:
  virtual void ProcessFixedBlock(
    FFHostEvents const& events, 
    FFFixedStereoBlock const& audioIn, 
    FFFixedStereoBlock& audioOut) = 0;

public:
  FFSplitProcessor(int maxHostSampleCount);

  void ProcessHostBlock(
    FFHostInputBlock const& input, 
    FFHostOutputBlock& output);
};