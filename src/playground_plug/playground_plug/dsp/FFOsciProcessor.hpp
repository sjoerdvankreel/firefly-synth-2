#pragma once

#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FFOsciBlock;
struct FFModuleProcState;

class FFOsciProcessor
{
  FBPhase _phase = {};

  void Generate(FFOsciBlock& block);
  void ApplyGLFOToGain(FFOsciBlock& block);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  void Process(FFModuleProcState const& state, int voice);
};