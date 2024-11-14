#pragma once
#include <playground_plug/shared/Toolchain.hpp>

class PluginProcessor
{
  float _phase = 0.0f;

public:
  void
  process(float const* FF_RESTRICT const* FF_RESTRICT in, float* FF_RESTRICT const* FF_RESTRICT out, int n);
};

