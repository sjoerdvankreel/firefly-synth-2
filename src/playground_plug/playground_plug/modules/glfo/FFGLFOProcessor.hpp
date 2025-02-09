#pragma once

#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;

class FFGLFOProcessor final
{
  FBPhase _phase = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOProcessor);
  int Process(FBModuleProcState& state);
  void Reset(FBModuleProcState const& state);

  int PlotLengthSamples(
    FBStaticTopo const& topo, FFScalarState const& state, 
    int moduleSlot, float sampleRate) const;
};