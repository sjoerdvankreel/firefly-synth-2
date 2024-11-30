#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/base/plug/FBPlugConfig.hpp>

#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/dsp/FBAutomationRamper.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>

#include <playground_plug/shared/FFPluginTopo.hpp>

class FFPluginProcessor:
public FBAutomationRamper<FFPluginProcessor>
{
  FBPhase _phase;
  FFProcMemory _memory;
  float const _sampleRate;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPluginProcessor);
  FFPluginProcessor(FBRuntimeTopo const& topo, int maxHostSampleCount, float sampleRate);
  void ProcessAutomation(FBPlugInputBlock const& input, FBPlugAudioBlock& output);
};