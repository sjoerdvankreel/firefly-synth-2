#pragma once
#include <playground_plug/base/shared/FFDefines.hpp>
#include <cstddef>

class FFPluginProcessor
{
  float _phase = 0.0f;
  float const _sampleRate = 0.0f;

public:

  FFPluginProcessor(float sampleRate);

  void 
  Process(FF_RAW_INPUT_BUFFER in, FF_RAW_OUTPUT_BUFFER out, std::size_t n);
};