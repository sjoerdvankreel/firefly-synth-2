#pragma once

#include <playground_plug/plug/shared/FFPluginBlocks.hpp>

struct FFPluginProcessorBlock;

class FFOscillatorProcessor
{
  float _phase = 0.0f;

public:
  void Process(int moduleSlot, FFPluginProcessorBlock& processorBlock);
};