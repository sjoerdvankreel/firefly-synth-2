#pragma once

#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;
enum class FFEnvStage { Delay, Attack, Hold, Decay, Sustain, Release };

struct FFEnvVoiceState
{
  bool on = {};
  FFEnvType type = {};
  int holdSamples = {};
  int delaySamples = {};
  int decaySamples = {};
  int attackSamples = {};
  int releaseSamples = {};
};

class FFEnvProcessor final
{
  int _position = 0;
  FFEnvVoiceState _voiceState = {};
  FFEnvStage _stage = FFEnvStage::Delay;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  void Process(FFModuleProcState const& state);
  void BeginVoice(FFModuleProcState const& state);
};