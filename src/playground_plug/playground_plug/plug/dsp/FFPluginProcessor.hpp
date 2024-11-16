#pragma once
#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <playground_plug/plug/dsp/FFOscillatorProcessor.hpp>

#include <array>
#include <vector>
#include <cstddef>

class FFPluginProcessor
{
  float const _sampleRate;
  int const _maxRemaining;
  FFAudioBlock _oscillatorBlock = {};
  FFOscillatorProcessor _oscillatorProcessor = {};
  std::array<std::vector<float>, 2> _remainingOutputBuffer = {};

public:
  FFPluginProcessor(int maxBlockSize, float sampleRate);
  void Process(FB_RAW_AUDIO_INPUT_BUFFER inputBuffer, FB_RAW_AUDIO_OUTPUT_BUFFER outputBuffer, int sampleCount);
};