#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>

#include <playground_base/dsp/FBDSPUtility.hpp>
#include <playground_base/dsp/FBHostBlockProcessor.hpp>
#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

#include <playground_plug/shared/FFPluginTopo.hpp>

class FFPluginProcessor:
public IFBHostBlockProcessor
{
  FBPhase _phase;
  float const _sampleRate;
  FFProcessorParamMemory _memory;
  FBFixedBlockProcessor _fixedProcessor;

  friend class FBFixedBlockProcessor;
  void ProcessFixed(FBFixedInputBlock const& input, FBFixedStereoBlock& output);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPluginProcessor);
  FFPluginProcessor(FBRuntimeTopo const& topo, int maxHostSampleCount, float sampleRate);
  void ProcessHost(FBHostInputBlock const& input, FBRawStereoBlockView& output) override;
};