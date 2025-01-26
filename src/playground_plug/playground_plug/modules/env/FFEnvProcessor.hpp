#pragma once

#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FFModuleProcState;
enum class FFEnvStage { Delay, Attack, Hold, Decay, Release, Count };

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
  int _finishedAt = -1;
  FFEnvVoiceState _voiceState = {};
  std::array<int, (int)FFEnvStage::Count> _stagePositions = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  int Process(FFModuleProcState const& state);
  void BeginVoice(FFModuleProcState const& state);
};