#pragma once

#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFOsciProcessor final
{
  FBPhase _phase = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  void Process(FFModuleProcState const& state);
  void BeginVoice(FFModuleProcState const& state) { _phase = {}; }
};