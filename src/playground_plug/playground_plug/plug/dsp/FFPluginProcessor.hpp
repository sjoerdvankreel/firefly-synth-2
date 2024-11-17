#pragma once

#include <playground_plug/base/dsp/FBPluginProcessor.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <playground_plug/plug/shared/FFPluginTopology.hpp>

#include <array>
#include <vector>

class FFPluginProcessor:
public FBPluginProcessor<FF_BLOCK_SIZE, FFPluginProcessor>
{
  FFPluginProcessors _processors = {};
  FFPluginProcessorBlock _processorBlock = {};

public:
  FFPluginProcessor(int maxBlockSize, float sampleRate);
  void ProcessInternal();
};