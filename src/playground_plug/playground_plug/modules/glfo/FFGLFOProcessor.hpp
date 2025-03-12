#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBTrackingPhase.hpp>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;
class FBGraphRenderState;

class FFGLFOProcessor final
{
  FBTrackingPhase _phase = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOProcessor);
  int Process(FBModuleProcState& state);
  void Reset(FBModuleProcState const& state);
};