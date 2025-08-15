#pragma once

#include <firefly_synth/dsp/shared/FFMarsagliaPRNG.hpp>
#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>
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
  FFLFOType _type = {};
  FFLFOSkewXMode _skewAXMode = {};
  FFLFOSkewYMode _skewAYMode = {};
  std::array<int, FFLFOBlockCount> _steps = {};
  std::array<float, FFLFOBlockCount> _phase = {};
  std::array<FFLFOWaveMode, FFLFOBlockCount> _waveMode = {};
  std::array<FFModulationOpType, FFLFOBlockCount> _opType = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};

  float _prevSeedNorm = {};
  int _prevSmoothSamples = {};
  bool _globalWasInit = false;
  std::array<float, FFLFOBlockCount> _prevPhaseNorm = {};
  std::array<float, FFLFOBlockCount> _prevStepsNorm = {};
  std::array<float, FFLFOBlockCount> _prevWaveModeNorm = {};

  bool _finished = false;
  bool _firstSample = true;
  float _lastOutputAll = 0.0f;
  std::array<float, FFLFOBlockCount> _lastOutputRaw = {};
  int _smoothSamples = 0;
  int _smoothSamplesProcessed = 0;
  FBBasicLPFilter _smoother = {};

  std::array<float, FFLFOBlockCount> _rateHzByBars = {};
  std::array<FFTrackingPhaseGenerator, FFLFOBlockCount> _phaseGens = {};
  std::array<FFNoiseGenerator<FFParkMillerPRNG, false>, FFLFOBlockCount> _uniNoiseGens = {};
  std::array<FFNoiseGenerator<FFParkMillerPRNG, true>, FFLFOBlockCount> _smoothUniNoiseGens = {};
  std::array<FFNoiseGenerator<FFMarsagliaPRNG<false>, false>, FFLFOBlockCount> _normNoiseGens = {};
  std::array<FFNoiseGenerator<FFMarsagliaPRNG<false>, true>, FFLFOBlockCount> _smoothNormNoiseGens = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOProcessor);

  template <bool Global>
  int Process(FBModuleProcState& state);
  template <bool Global>
  void BeginVoiceOrBlock(
    bool graph, int graphIndex, int graphSampleCount, 
    FFLFOExchangeState const* exchangeState, FBModuleProcState& state);
};