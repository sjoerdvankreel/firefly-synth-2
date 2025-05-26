#pragma once

#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBBasicLPFilter.hpp>

#include <array>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;
class FBGraphRenderState;

class FFEnvProcessor final
{
  bool _on = {};
  bool _exp = {};
  bool _sync = {};
  bool _sustain = {};
  int _loopStart = {};
  int _loopLength = {};
  int _releasePoint = {};

  bool _released = false;
  bool _finished = false;
  int _smoothSamples = 0;
  int _lengthSamples = 0;
  int _positionSamples = 0;
  int _lengthSamplesUpToRelease = 0;
  float _lastOverall = 0.0f;
  float _lastBeforeRelease = 0.0f;

  FBBasicLPFilter _smoother = {};
  std::array<int, (int)FFEnvStageCount> _stageSamples = {};
  std::array<int, (int)FFEnvStageCount> _stagePositions = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};