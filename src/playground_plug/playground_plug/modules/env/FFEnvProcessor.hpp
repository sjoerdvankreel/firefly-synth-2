#pragma once

#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>

#include <array>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;

enum class FFEnvStage { Delay, Attack, Hold, Decay, Release, Smooth, Count };

struct FFEnvVoiceState
{
  bool on = {};
  bool sync = {};
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
  bool _finished = false;
  int _lengthSamples = 0;
  int _positionSamples = 0;
  float _lastDAHDSR = 0.0f;
  float _lastBeforeRelease = 0.0f;
  FBOnePoleFilter _smoother = {};
  FFEnvVoiceState _voiceState = {};
  std::array<int, (int)FFEnvStage::Count> _stagePositions = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);

  int PlotLengthSamples(
    FBStaticTopo const& topo, FFScalarState const& state,
    int moduleSlot, float sampleRate, float bpm) const;
};