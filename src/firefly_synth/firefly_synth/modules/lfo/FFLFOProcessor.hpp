#pragma once

#include <firefly_synth/dsp/shared/FFPhaseGenerator.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFLFOProcessor final
{
  bool _sync = {};
  FFLFOType _type = {};
  FFLFOSkewXMode _skewAXMode = {};
  FFLFOSkewYMode _skewAYMode = {};
  std::array<int, FFLFOBlockCount> _steps = {};
  std::array<FFLFOOpType, FFLFOBlockCount> _opType = {};
  std::array<FFLFOWaveMode, FFLFOBlockCount> _waveMode = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};

  int _smoothSamples = 0;
  FBBasicLPFilter _smoother = {};

  std::array<float, FFLFOBlockCount> _rateHzByBars = {};
  std::array<FFTimeVectorPhaseGenerator, FFLFOBlockCount> _phaseGens = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOProcessor);

  template <bool Global>
  int Process(FBModuleProcState& state);
  template <bool Global>
  void BeginVoiceOrBlock(bool graph, int graphIndex, int graphSampleCount /*todo auto*/, FBModuleProcState& state);
};