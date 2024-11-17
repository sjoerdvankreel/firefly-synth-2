#pragma once

#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/dsp/FBPluginProcessor.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>

class FFPluginProcessor:
public FBPluginProcessor<FFPluginProcessor, FF_BLOCK_SIZE>
{
  FFPluginProcessors _processors = {};
  FFProcessorBlock _processorBlock = {};

public:
  FFPluginProcessor(int maxHostBlockSize, float sampleRate);
  void ProcessPluginBlock();
};