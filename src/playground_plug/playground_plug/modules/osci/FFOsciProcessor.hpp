#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

struct FFOsciVoiceState final
{
  bool on = {};
  int note = {};
  float key = {}; // TODO floating key
  FFOsciType type = {};
};

class FFOsciProcessor final
{
  FBPhase _phase = {};
  FFOsciVoiceState _voiceState = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  void Process(FFModuleProcState const& state);
  void BeginVoice(FFModuleProcState const& state);
};