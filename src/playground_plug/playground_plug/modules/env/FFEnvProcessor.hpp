#pragma once

#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>

#include <array>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;
class FBGraphRenderState;

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
  bool _released = false;
  bool _finished = false;
  float _lastDAHDSR = 0.0f;
  float _lastBeforeRelease = 0.0f;
  int _lengthSamples = 0;
  int _positionSamples = 0;
  int _lengthSamplesUpToRelease = 0;
  FBOnePoleFilter _smoother = {};
  FFEnvVoiceState _voiceState = {};
  std::array<int, (int)FFEnvStage::Count> _stagePositions = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);
};