#pragma once

#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBOnePoleLPFilter.hpp>

#include <array>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;
class FBGraphRenderState;

enum class FFEnvStage { Delay, Attack, Hold, Decay, Release, Smooth, Count };

class FFEnvProcessor final
{
  bool _on = {};
  bool _sync = {};
  FFEnvType _type = {};
  FFEnvMode _mode = {};
  int _holdSamples = {};
  int _delaySamples = {};
  int _decaySamples = {};
  int _attackSamples = {};
  int _releaseSamples = {};
  int _smoothingSamples = {};

  bool _released = false;
  bool _finished = false;
  int _lengthSamples = 0;
  int _positionSamples = 0;
  int _lengthSamplesUpToRelease = 0;
  float _lastDAHDSR = 0.0f;
  float _lastBeforeRelease = 0.0f;
  FBOnePoleLPFilter _smoother = {};
  std::array<int, (int)FFEnvStage::Count> _stagePositions = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};