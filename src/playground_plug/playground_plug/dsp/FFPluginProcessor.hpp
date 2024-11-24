#pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/dsp/FBDSPUtility.hpp>
#include <playground_base/dsp/FBFixedBlockProcessor.hpp>
#include <playground_plug/shared/FFPluginTopo.hpp>

class FFPluginProcessor
{
  FBPhase _phase;
  float const _sampleRate;
  FFProcParamMemory _memory;
  FBFixedBlockProcessor _fixedProcessor;

  friend class FBFixedBlockProcessor;
  void ProcessFixed(FBFixedInputBlock const& input, FFFixedStereoBlock& output);

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFPluginProcessor);
  FFPluginProcessor(int maxHostSampleCount, float sampleRate);
  void ProcessHost(FBHostInputBlock const& input, FFRawStereoBlockView& output);
};