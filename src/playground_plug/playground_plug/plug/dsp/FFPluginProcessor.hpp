#pragma once

#include <playground_plug/base/shared/FFHostBlock.hpp>
#include <playground_plug/base/shared/FFSignalBlock.hpp>
#include <playground_plug/base/dsp/FFFixedBlockProcessor.hpp>

class FFPluginProcessor
{
  float _phase;
  float const _sampleRate;
  FFFixedBlockProcessor _fixedProcessor;
  friend class FFFixedBlockProcessor;

  void ProcessFixed(
    FFFixedInputBlock const& input,
    FFFixedStereoBlock& output);

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFPluginProcessor);
  FFPluginProcessor(int maxHostSampleCount, float sampleRate);

  void ProcessHost(
    FFHostInputBlock const& input,
    FFRawStereoBlock& output);
};