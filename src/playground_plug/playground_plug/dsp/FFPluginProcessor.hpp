#pragma once

#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base/dsp/fixed/FBFixedConfig.hpp>

#include <playground_base/dsp/FBAutomationRamper.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>

#include <playground_plug/shared/FFPluginTopo.hpp>

class FFPluginProcessor:
public FBAutomationRamper<FFPluginProcessor>
{
  float _phase = 0;
  FFProcMemory _memory;
  float const _sampleRate;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPluginProcessor);
  FFPluginProcessor(FBRuntimeTopo const& topo, int maxHostSampleCount, float sampleRate);
  void ProcessAutomation(FBFixedInputBlock const& input, FBFixedAudioBlock& output);
};