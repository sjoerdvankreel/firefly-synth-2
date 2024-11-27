#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>

#include <playground_base/dsp/FBDSPUtility.hpp>
#include <playground_base/dsp/FBAutomationRamper.hpp>
#include <playground_base/dsp/FBHostBlockProcessor.hpp>

#include <playground_plug/shared/FFPluginTopo.hpp>

class FFPluginProcessor:
public FBAutomationRamper<FFPluginProcessor>
{
  FBPhase _phase;
  float const _sampleRate;
  FFDenseParamMemory _memory;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPluginProcessor);
  FFPluginProcessor(FBRuntimeTopo const& topo, int maxHostSampleCount, float sampleRate);
  void ProcessAutomation(FBPlugInputBlock const& input, FBPlugAudioBlock& output);
};