#pragma once
#include <playground_plug/plug/dsp/FFPluginBlocks.hpp>

class FFOscillatorProcessor
{
  float _phase = 0.0f;
public:
  void Process(float sampleRate, FFAudioBlock& block);
};