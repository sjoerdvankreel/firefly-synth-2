#pragma once

#include <firefly_synth/dsp/shared/FFPhaseGenerator.hpp>
#include <firefly_synth/dsp/shared/FFNoiseGenerator.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;
struct FFLFOExchangeState;

class FFLFOProcessor final
{
  bool _sync = {};
  float _phaseB = {};
  FFLFOType _type = {};
  FFLFOSkewXMode _skewAXMode = {};
  FFLFOSkewYMode _skewAYMode = {};
  std::array<int, FFLFOBlockCount> _steps = {};
  std::array<FFLFOOpType, FFLFOBlockCount> _opType = {};
  std::array<FFLFOWaveMode, FFLFOBlockCount> _waveMode = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};

  float _prevSeedNorm = {};
  float _prevPhaseBNorm = {};
  int _prevSmoothSamples = {};
  bool _globalWasInit = false;
  std::array<float, FFLFOBlockCount> _prevStepsNorm = {};
  std::array<float, FFLFOBlockCount> _prevWaveModeNorm = {};

  bool _finished = false;
  bool _firstSample = true;
  float _lastOutput = 0.0f;
  int _smoothSamples = 0;
  int _smoothSamplesProcessed = 0;
  FBBasicLPFilter _smoother = {};

  std::array<float, FFLFOBlockCount> _rateHzByBars = {};
  std::array<FFTrackingPhaseGenerator, FFLFOBlockCount> _phaseGens = {};
  std::array<FFNoiseGenerator<false>, FFLFOBlockCount> _noiseGens = {};
  std::array<FFNoiseGenerator<true>, FFLFOBlockCount> _smoothNoiseGens = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOProcessor);

  template <bool Global>
  int Process(FBModuleProcState& state);
  template <bool Global>
  void BeginVoiceOrBlock(
    bool graph, int graphIndex, int graphSampleCount, 
    FFLFOExchangeState const* exchangeState, FBModuleProcState& state);
};