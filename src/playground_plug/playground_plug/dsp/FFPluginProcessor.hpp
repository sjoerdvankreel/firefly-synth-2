#pragma once

#include <playground_plug/base/shared/FFHostBlock.hpp>
#include <playground_plug/base/shared/FFSignalBlock.hpp>
#include <playground_plug/base/dsp/FFDSPUtility.hpp>
#include <playground_plug/base/dsp/FBFixedBlockProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginTopo.hpp>

class FFPluginProcessor
{
  FFPhase _phase;
  float const _sampleRate;
  FFProcParamMemory _memory;
  FBFixedBlockProcessor _fixedProcessor;

  friend class FBFixedBlockProcessor;
  void ProcessFixed(FFFixedInputBlock const& input, FFFixedStereoBlock& output);

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFPluginProcessor);
  FFPluginProcessor(int maxHostSampleCount, float sampleRate);
  void ProcessHost(FFHostInputBlock const& input, FFRawStereoBlockView& output);
};