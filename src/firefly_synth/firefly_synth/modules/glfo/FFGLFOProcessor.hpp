#pragma once

#include <firefly_synth/dsp/shared/FFTrackingPhaseGenerator.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;
class FBGraphRenderState;

class FFGLFOProcessor final
{
  FFTrackingPhaseGenerator _phase = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOProcessor);
  void Reset(FBModuleProcState& state);
  int Process(FBModuleProcState& state);
};