#pragma once

#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>

#include <array>

struct FBModuleProcState;
enum class FFEnvStage { Delay, Attack, Hold, Decay, Release, Smooth, Count };

struct FFEnvVoiceState
{
  bool on = {};
  FFEnvType type = {};  
  FFEnvMode mode = {};
  int holdSamples = {};
  int delaySamples = {};
  int decaySamples = {};
  int attackSamples = {};
  int releaseSamples = {};
  int smoothingSamples = {};
};

class FFEnvProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  int Process(FBModuleProcState const& state);
  void BeginVoice(FBModuleProcState const& state);

private:
  int _position = 0;
  bool _finished = false;
  float _lastDAHDSR = 0.0f;
  FBOnePoleFilter _smoother = {};
  FFEnvVoiceState _voiceState = {};
  std::array<int, (int)FFEnvStage::Count> _stagePositions = {};
};