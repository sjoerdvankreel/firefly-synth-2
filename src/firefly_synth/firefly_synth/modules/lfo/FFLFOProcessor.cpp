#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

static auto const InvLogHalf = 1.0f / std::log(0.5f);

static inline FBBatch<float>
SkewScaleUnipolar(FBBatch<float> in, FBBatch<float> amt)
{
  return in * (1.0f - amt);
}

static inline FBBatch<float>
SkewScaleBipolar(FBBatch<float> in, FBBatch<float> amt)
{
  return FBToUnipolar(FBToBipolar(in) * (1.0f - amt));
}

static inline FBBatch<float>
SkewExpUnipolar(FBBatch<float> in, FBBatch<float> amt)
{ 
  return xsimd::pow(in, xsimd::log(0.001f + (amt * 0.999f)) * InvLogHalf);
}

static inline FBBatch<float>
SkewExpBipolar(FBBatch<float> in, FBBatch<float> amt)
{
  auto bp = FBToBipolar(in);
  auto exp = xsimd::log(0.001f + (amt * 0.999f)) * InvLogHalf;
  return FBToUnipolar(xsimd::sign(bp) * xsimd::pow(xsimd::abs(bp), exp));
}

static inline FBBatch<float>
SkewY(FFLFOSkewYMode mode, FBBatch<float> in, FBBatch<float> amt)
{
  switch (mode)
  {
  case FFLFOSkewYMode::ExpBipolar: return SkewExpBipolar(in, amt);
  case FFLFOSkewYMode::ExpUnipolar: return SkewExpUnipolar(in, amt);
  default: FB_ASSERT(false); return {};
  }
}

static inline FBBatch<float>
SkewX(FFLFOSkewXMode mode, FBBatch<float> in, FBBatch<float> amt)
{
  switch (mode)
  {
  case FFLFOSkewXMode::ExpBipolar: return SkewExpBipolar(in, amt);
  case FFLFOSkewXMode::ExpUnipolar: return SkewExpUnipolar(in, amt);
  case FFLFOSkewXMode::ScaleBipolar: return SkewScaleBipolar(in, amt);
  case FFLFOSkewXMode::ScaleUnipolar: return SkewScaleUnipolar(in, amt);
  default: FB_ASSERT(false); return {};
  }
}

template <bool Global>
void
FFLFOProcessor::BeginVoiceOrBlock(
  bool graph, int graphIndex, int graphSampleCount, 
  FFLFOExchangeState const* exchangeState, FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gLFO[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vLFO[state.moduleSlot]; });
  auto const& topo = state.topo->static_.modules[(int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO)];

  auto const& stepsNorm = params.block.steps;
  auto const& opTypeNorm = params.block.opType;
  auto const& waveModeNorm = params.block.waveMode;
  auto const& rateBarsNorm = params.block.rateBars;
  float seedNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.seed[0], voice);
  float typeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.type[0], voice);
  float syncNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.sync[0], voice);
  float phaseBNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.phaseB[0], voice);
  float skewAXModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewAXMode[0], voice);
  float skewAYModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewAYMode[0], voice);
  float smoothBarsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.smoothBars[0], voice);
  float smoothTimeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.smoothTime[0], voice);

  _smoother = {};
  _rateHzByBars = {};
  _smoothSamples = 0;
  _lastOutput = 0.0f;
  _finished = false;
  _firstSample = true;
  _smoothSamplesProcessed = 0;

  _graph = graph;
  _graphSamplesProcessed = 0;
  _graphSampleCount = graphSampleCount;

  _sync = topo.NormalizedToBoolFast(FFLFOParam::Sync, syncNorm);
  _type = topo.NormalizedToListFast<FFLFOType>(FFLFOParam::Type, typeNorm);
  _skewAXMode = topo.NormalizedToListFast<FFLFOSkewXMode>(FFLFOParam::SkewAXMode, skewAXModeNorm);
  _skewAYMode = topo.NormalizedToListFast<FFLFOSkewYMode>(FFLFOParam::SkewAYMode, skewAYModeNorm);

  if (_sync)
    _smoothSamples = topo.NormalizedToBarsSamplesFast(
      FFLFOParam::SmoothBars, smoothBarsNorm, state.input->sampleRate, state.input->bpm);
  else
    _smoothSamples = topo.NormalizedToLinearTimeSamplesFast(
      FFLFOParam::SmoothTime, smoothTimeNorm, state.input->sampleRate);
  _smoother.SetCoeffs(_smoothSamples);

  bool shouldInit = true;
  if constexpr (Global)
  {
    shouldInit = graph || !_globalWasInit;
    shouldInit |= _prevSeedNorm != seedNorm;
    shouldInit |= _prevPhaseBNorm != phaseBNorm;
    _prevSeedNorm = seedNorm;
    _prevPhaseBNorm = phaseBNorm;
    for (int i = 0; i < FFLFOBlockCount; i++)
    {
      shouldInit |= _prevStepsNorm[i] != stepsNorm[i].Value();
      _prevStepsNorm[i] = stepsNorm[i].Value();
    }
  }

  int seed = topo.NormalizedToDiscreteFast(FFLFOParam::Seed, seedNorm);
  float floatSeed = seed / (FFLFOMaxSeed + 1.0f);

  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    bool blockActive = !graph || graphIndex == i || graphIndex == FFLFOBlockCount;
    _opType[i] = !blockActive ? 
      FFLFOOpType::Off : 
      graph && graphIndex != FFLFOBlockCount ?
      FFLFOOpType::Add :
      topo.NormalizedToListFast<FFLFOOpType>(
      FFLFOParam::OpType,
      FFSelectDualProcBlockParamNormalized<Global>(opTypeNorm[i], voice));
    _steps[i] = topo.NormalizedToDiscreteFast(
      FFLFOParam::Steps,
      FFSelectDualProcBlockParamNormalized<Global>(stepsNorm[i], voice));
    _waveMode[i] = topo.NormalizedToListFast<FFLFOWaveMode>(
      FFLFOParam::WaveMode,
      FFSelectDualProcBlockParamNormalized<Global>(waveModeNorm[i], voice));
    
    if (_sync)
      _rateHzByBars[i] = topo.NormalizedToBarsFreqFast(FFLFOParam::RateBars,
        FFSelectDualProcBlockParamNormalized<Global>(rateBarsNorm[i], voice), state.input->bpm);

    bool movingGraph = exchangeState != nullptr && (
      graphIndex == FFLFOBlockCount || 
      _waveMode[i] == FFLFOWaveModeFreeRandom || 
      _waveMode[i] == FFLFOWaveModeFreeSmooth);

    if (movingGraph)
    {
      _phaseGens[i] = FFTrackingPhaseGenerator(exchangeState->phases[i]);
      _noiseGens[i].Init(exchangeState->noiseState[i], _steps[i] + 1);
      _smoothNoiseGens[i].Init(exchangeState->smoothNoiseState[i], _steps[i] + 1);
    }
    else
    {
      if (shouldInit)
      {
        _noiseGens[i].Init(floatSeed, _steps[i] + 1);
        _smoothNoiseGens[i].Init(floatSeed, _steps[i] + 1);
        if (i == 1)
          _phaseGens[i] = FFTrackingPhaseGenerator(topo.NormalizedToIdentityFast(FFLFOParam::PhaseB, phaseBNorm));
        else
          _phaseGens[i] = FFTrackingPhaseGenerator(0.0f);
      }
    }
  }
  _globalWasInit = true;
}

template <bool Global>
int
FFLFOProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& procParams = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gLFO[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vLFO[state.moduleSlot]; });
  auto& dspState = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->dsp.global.gLFO[state.moduleSlot]; },
    [procState, voice, &state]() { return &procState->dsp.voice[voice].vLFO[state.moduleSlot]; });
  auto& output = dspState.output;
  auto const& topo = state.topo->static_.modules[(int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO)];

  if (_type == FFLFOType::Off)
  {
    output.Fill(0.0f);
    return FBFixedBlockSamples;
  }

  if constexpr (!Global)
  {
    if (_finished)
    {
      output.Fill(_smoother.State());
      return _graph? 0: FBFixedBlockSamples;
    }
  }

  float sampleRate = state.input->sampleRate;
  auto const& minNorm = procParams.acc.min;
  auto const& maxNorm = procParams.acc.max;
  auto const& rateHzNorm = procParams.acc.rateHz;
  auto const& skewAXAmtNorm = FFSelectDualProcAccParamNormalized<Global>(procParams.acc.skewAXAmt[0], voice);
  auto const& skewAYAmtNorm = FFSelectDualProcAccParamNormalized<Global>(procParams.acc.skewAYAmt[0], voice);

  FBSArray<float, FBFixedBlockSamples> skewAXAmtPlain;
  FBSArray<float, FBFixedBlockSamples> skewAYAmtPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> minPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> maxPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> rateHzPlain;

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    if(_skewAXMode != FFLFOSkewXMode::Off)
      skewAXAmtPlain.Store(s, topo.NormalizedToIdentityFast(FFLFOParam::SkewAXAmt, skewAXAmtNorm, s));
    if(_skewAYMode != FFLFOSkewYMode::Off)
      skewAYAmtPlain.Store(s, topo.NormalizedToIdentityFast(FFLFOParam::SkewAYAmt, skewAYAmtNorm, s));

    for (int i = 0; i < FFLFOBlockCount; i++)
    {
      if (_opType[i] != FFLFOOpType::Off)
      {
        minPlain[i].Store(s, topo.NormalizedToIdentityFast(FFLFOParam::Min,
          FFSelectDualProcAccParamNormalized<Global>(minNorm[i], voice), s));
        maxPlain[i].Store(s, topo.NormalizedToIdentityFast(FFLFOParam::Max,
          FFSelectDualProcAccParamNormalized<Global>(maxNorm[i], voice), s));
        if (_sync)
          rateHzPlain[i].Store(s, _rateHzByBars[i]);
        else
          rateHzPlain[i].Store(s, topo.NormalizedToLinearFast(FFLFOParam::RateHz,
            FFSelectDualProcAccParamNormalized<Global>(rateHzNorm[i], voice), s));
      }
    }
  }

  int s = 0;
  output.Fill(0.0f);
  bool oneShotFinished = !Global && _type == FFLFOType::SnapOrOneShot && _phaseGens[0].CycledOnce();
  for (; s < FBFixedBlockSamples && !oneShotFinished; s += FBSIMDFloatCount)
  {
    for (int i = 0; i < FFLFOBlockCount; i++)
    {
      if (_opType[i] != FFLFOOpType::Off)
      {
        bool wrapped;
        auto incr = rateHzPlain[i].Load(s) / sampleRate;
        auto phase = _phaseGens[i].NextBatch(incr, wrapped);

        // reset before wrap point, otherwise we get a tiny piece of the new random table
        if (wrapped && (_waveMode[i] == FFLFOWaveModeFreeRandom || _waveMode[i] == FFLFOWaveModeFreeSmooth))
        {
          _phaseGens[i].Reset();
          phase = _phaseGens[i].NextBatch(incr, wrapped);
          if(_waveMode[i] == FFLFOWaveModeFreeRandom)
            _noiseGens[i].Init(_noiseGens[i].Prng().NextScalar(), _steps[i]);
          if(_waveMode[i] == FFLFOWaveModeFreeSmooth)
            _smoothNoiseGens[i].Init(_smoothNoiseGens[i].Prng().NextScalar(), _steps[i]);
        }

        if (i == 0 && _skewAXMode != FFLFOSkewXMode::Off)
        {
          auto skewAXAmt = skewAXAmtPlain.Load(s);
          phase = SkewX(_skewAXMode, phase, skewAXAmt);
        }

        FBBatch<float> lfo = {};
        switch (_waveMode[i])
        {
        case FFLFOWaveModeSaw: lfo = phase; break;
        case FFLFOWaveModeTri: lfo = 1.0f - xsimd::abs(FBToBipolar(phase)); break;
        case FFLFOWaveModeSqr: lfo = FBToUnipolar(xsimd::sign(FBToBipolar(phase))); break;
        case FFLFOWaveModeRandom: lfo = _noiseGens[i].AtBatch(phase); break;
        case FFLFOWaveModeFreeRandom: lfo = _noiseGens[i].AtBatch(phase); break;
        case FFLFOWaveModeSmooth: lfo = _smoothNoiseGens[i].AtBatch(phase); break;
        case FFLFOWaveModeFreeSmooth: lfo = _smoothNoiseGens[i].AtBatch(phase); break;
        default: lfo = FBToUnipolar(FFCalcTrig(_waveMode[i], phase * 2.0f * FBPi)); break;
        }

        if (i == 0 && _skewAYMode != FFLFOSkewYMode::Off)
        {
          auto skewAYAmt = skewAYAmtPlain.Load(s);
          lfo = SkewY(_skewAYMode, lfo, skewAYAmt);
        }

        if (_steps[i] > 1 && !(FFLFOWaveModeRandom <= _waveMode[i] && _waveMode[i] <= FFLFOWaveModeFreeSmooth))
        {
          lfo = xsimd::clip(lfo, FBBatch<float>(0.0f), FBBatch<float>(0.9999f));
          lfo = xsimd::floor(lfo * static_cast<float>(_steps[i])) / (_steps[i] - 1.0f);
        }

        auto min = minPlain[i].Load(s);
        auto max = maxPlain[i].Load(s);
        lfo = min + (max - min) * lfo;

        switch (_opType[i])
        {
        case FFLFOOpType::Mul: output.Mul(s, lfo); break;
        case FFLFOOpType::Add: output.Store(s, xsimd::clip(output.Load(s) + lfo, FBBatch<float>(0.0f), FBBatch<float>(1.0f))); break;
        case FFLFOOpType::Stack: output.Add(s, (1.0f - output.Load(s)) * lfo); break;
        default: FB_ASSERT(false); break;
        }
      }
    }

    _lastOutput = output.Get(s + FBSIMDFloatCount - 1);
    oneShotFinished = !Global && _type == FFLFOType::SnapOrOneShot && _phaseGens[0].CycledOnce();
  }

  for (; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    output.Store(s, _lastOutput);

  for (s = 0; s < FBFixedBlockSamples; s++)
  {
    float in = output.Get(s);
    if (_firstSample)
    {
      _smoother.State(in);
      _firstSample = false;
    }
    output.Set(s, _smoother.Next(in));
    if constexpr (!Global)
    {
      if (oneShotFinished)
      {
        _smoothSamplesProcessed++;
        _finished |= _smoothSamplesProcessed >= _smoothSamples;
      }
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    _graphSamplesProcessed += FBFixedBlockSamples;
    return std::clamp(_graphSampleCount - _graphSamplesProcessed, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = *FFSelectDualState<Global>(
    [exchangeToGUI, &state]() { return &exchangeToGUI->global.gLFO[state.moduleSlot]; },
    [exchangeToGUI, &state, voice]() { return &exchangeToGUI->voice[voice].vLFO[state.moduleSlot]; });
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples[FFLFOBlockCount] = 0;
  exchangeDSP.positionSamples[FFLFOBlockCount] = 0;
  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    exchangeDSP.phases[i] = _phaseGens[i].CurrentScalar();
    exchangeDSP.noiseState[i] = _noiseGens[i].PrngStateAtInit();
    exchangeDSP.smoothNoiseState[i] = _smoothNoiseGens[i].PrngStateAtInit();
    exchangeDSP.lengthSamples[i] = rateHzPlain[i].Last() > 0.0f ? FBFreqToSamples(rateHzPlain[i].Last(), sampleRate) : 0;
    exchangeDSP.lengthSamples[FFLFOBlockCount] = std::max(exchangeDSP.lengthSamples[i], exchangeDSP.lengthSamples[FFLFOBlockCount]);

    // 0: the lines move, so the position indicator stays fixed.
    if (_waveMode[i] == FFLFOWaveModeFreeRandom || _waveMode[i] == FFLFOWaveModeFreeSmooth)
      exchangeDSP.positionSamples[i] = 0;
    else
      exchangeDSP.positionSamples[i] = _phaseGens[i].PositionSamplesCurrentCycle();
  }

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gLFO[state.moduleSlot]; },
    [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vLFO[state.moduleSlot]; });
  FFSelectDualExchangeState<Global>(exchangeParams.acc.skewAXAmt[0], voice) = skewAXAmtNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.skewAYAmt[0], voice) = skewAYAmtNorm.Last();
  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    FFSelectDualExchangeState<Global>(exchangeParams.acc.min[i], voice) = FFSelectDualProcAccParamNormalized<Global>(minNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.max[i], voice) = FFSelectDualProcAccParamNormalized<Global>(maxNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.rateHz[i], voice) = FFSelectDualProcAccParamNormalized<Global>(rateHzNorm[i], voice).Last();
  }
   
  return FBFixedBlockSamples;
}

template int FFLFOProcessor::Process<true>(FBModuleProcState&);
template int FFLFOProcessor::Process<false>(FBModuleProcState&);
template void FFLFOProcessor::BeginVoiceOrBlock<true>(bool, int, int, FFLFOExchangeState const*, FBModuleProcState&);
template void FFLFOProcessor::BeginVoiceOrBlock<false>(bool, int, int, FFLFOExchangeState const*, FBModuleProcState&);