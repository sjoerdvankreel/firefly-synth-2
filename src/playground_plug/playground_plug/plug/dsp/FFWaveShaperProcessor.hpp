#pragma once

#include <playground_plug/plug/shared/FFPluginBlocks.hpp>

struct FFPluginProcessorBlock;

class FFWaveShaperProcessor
{
public:
  void Process(int moduleSlot, float sampleRate, FFPluginProcessorBlock& processorBlock);
};