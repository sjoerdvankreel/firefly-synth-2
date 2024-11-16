#pragma once
#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <cstddef>

class FFPluginProcessor
{
  float const _sampleRate = 0.0f;

public:

  FFPluginProcessor(float sampleRate);

  void 
  Process(FB_RAW_INPUT_BUFFER in, FB_RAW_OUTPUT_BUFFER out, std::size_t sampleCount);
};