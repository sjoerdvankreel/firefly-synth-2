#pragma once
#include <playground_plug/plug/dsp/FFBuffers.hpp>
#include <playground_plug/base/shared/FBDefines.hpp>
#include <cstddef>

class FFProcessor
{
  float _phase = 0.0f;
  float const _sampleRate = 0.0f;
  FFFixedStereoBuffer _fixedBuffer = {};

public:

  FFProcessor(float sampleRate);

  void 
  Process(FB_RAW_INPUT_BUFFER in, FB_RAW_OUTPUT_BUFFER out, std::size_t sampleCount);
};