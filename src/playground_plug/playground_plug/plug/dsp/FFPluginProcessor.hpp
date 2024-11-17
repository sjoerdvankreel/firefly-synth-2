#pragma once

#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/dsp/FBPluginProcessor.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>

class FFPluginProcessor:
public FBPluginProcessor<FFPluginProcessor, FF_BLOCK_SIZE>
{
  FFProcessorBlock _block = {};
  FFPluginProcessors _processors = {};

public:
  FFPluginProcessor(int maxBlockSize, float sampleRate);
  void ProcessInternal();
};