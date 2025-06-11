#pragma once

#include <firefly_synth/dsp/shared/FFTrackingPhaseGenerator.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;
class FBGraphRenderState;

class FFGLFOProcessor final
{
  bool _on = {};
  FFTrackingPhaseGenerator _phase = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOProcessor);
  int Process(FBModuleProcState& state);
  void BeginBlock(bool graph, FBModuleProcState& state);
};