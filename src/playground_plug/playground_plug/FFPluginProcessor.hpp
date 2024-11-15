#pragma once
#include <playground_plug/shared/FFDefines.hpp>
#include <cstddef>

class FFPluginProcessor
{
  float _phase = 0.0f;

public:

  void 
  Process(FF_RAW_INPUT_BUFFER in, FF_RAW_OUTPUT_BUFFER out, std::size_t n);
};