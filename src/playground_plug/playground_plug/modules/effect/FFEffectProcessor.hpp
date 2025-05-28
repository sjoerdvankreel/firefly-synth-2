#pragma once

#include <playground_plug/modules/effect/FFEffectTopo.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFEffectProcessor final
{
  bool _on = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};