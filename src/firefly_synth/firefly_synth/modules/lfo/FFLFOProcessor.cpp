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

static inline FBBatch<float>
SkewY(FFLFOSkewYMode mode, FBBatch<float> in, FBBatch<float> amt)
{
  switch (mode)
  {
  case FFLFOSkewYMode::ExpBipolar: return FFSkewExpBipolar(in, amt);
  case FFLFOSkewYMode::ExpUnipolar: return FFSkewExpUnipolar(in, amt);
  default: FB_ASSERT(false); return {};
  }
}

static inline FBBatch<float>
SkewX(FFLFOSkewXMode mode, FBBatch<float> in, FBBatch<float> amt)
{
  switch (mode)
  {
  case FFLFOSkewXMode::ExpBipolar: return FFSkewExpBipolar(in, amt);
  case FFLFOSkewXMode::ExpUnipolar: return FFSkewExpUnipolar(in, amt);
  case FFLFOSkewXMode::ScaleBipolar: return FFSkewScaleBipolar(in, amt);
  case FFLFOSkewXMode::ScaleUnipolar: return FFSkewScaleUnipolar(in, amt);
  default: FB_ASSERT(false); return {};
  }
}

template <bool Global>
void
FFLFOProcessor::BeginVoiceOrBlock(
  FBModuleProcState& state,
  FFLFOExchangeState const* exchangeFromDSP,
  bool graph, int graphIndex, int graphSampleCount)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gLFO[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vLFO[state.moduleSlot]; });
  auto const& topo = state.topo->static_->modules[(int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO)];

  auto const& stepsNorm = params.block.steps;
  auto const& phaseNorm = params.block.phase;
  auto const& opTypeNorm = params.block.opType;
  auto const& waveModeNorm = params.block.waveMode;
  auto const& rateBarsNorm = params.block.rateBars;
  float seedNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.seed[0], voice);
  float typeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.type[0], voice);
  float syncNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.sync[0], voice);
  float skewAXModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewAXMode[0], voice);
  float skewAYModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewAYMode[0], voice);
  float smoothBarsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.smoothBars[0], voice);
  float smoothTimeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.smoothTime[0], voice);

  _rateHzByBars = {};
  _smoothSamples = 0;
  _lastOutputAll = 0.0f;
  _lastOutputRaw = {};
  _finished = false;
  _smoothSamplesProcessed = 0;

  _graph = graph;
  _graphSamplesProcessed = 0;
  _graphSampleCount = graphSampleCount;

  _sync = topo.NormalizedToBoolFast(FFLFOParam::Sync, syncNorm);
  _type = topo.NormalizedToListFast<FFLFOType>(FFLFOParam::Type, typeNorm);
  _skewAXMode = topo.NormalizedToListFast<FFLFOSkewXMode>(FFLFOParam::SkewAXMode, skewAXModeNorm);
  _skewAYMode = topo.NormalizedToListFast<FFLFOSkewYMode>(FFLFOParam::SkewAYMode, skewAYModeNorm);

  if (graph && graphIndex != FFLFOBlockCount)
    _smoothSamples = 0;
  else
  {
    if (_sync)
      _smoothSamples = topo.NormalizedToBarsSamplesFast(
        FFLFOParam::SmoothBars, smoothBarsNorm, state.input->sampleRate, state.input->bpm);
    else
      _smoothSamples = topo.NormalizedToLinearTimeSamplesFast(
        FFLFOParam::SmoothTime, smoothTimeNorm, state.input->sampleRate);
  }

  bool shouldInit = false;
  if constexpr (!Global)
  {
    shouldInit = true;
  }
  else
  {
    shouldInit = graph || !_globalWasInit;
    shouldInit |= _prevSeedNorm != seedNorm;
    shouldInit |= _prevSmoothSamples != _smoothSamples;
    _prevSeedNorm = seedNorm;
    _prevSmoothSamples = _smoothSamples;
    for (int i = 0; i < FFLFOBlockCount; i++)
    {
      shouldInit |= _prevPhaseNorm[i] != phaseNorm[i].Value();
      shouldInit |= _prevStepsNorm[i] != stepsNorm[i].Value();
      shouldInit |= _prevWaveModeNorm[i] != waveModeNorm[i].Value();
      _prevStepsNorm[i] = stepsNorm[i].Value();
      _prevPhaseNorm[i] = phaseNorm[i].Value();
      _prevWaveModeNorm[i] = waveModeNorm[i].Value();
    }
  }

  if (shouldInit)
  {
    _firstSample = true;
    _smoother = {};
    _smoother.SetCoeffs(_smoothSamples);
  }

  int seed = topo.NormalizedToDiscreteFast(FFLFOParam::Seed, seedNorm);
  float floatSeed = seed / (FFLFOMaxSeed + 1.0f);

  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    bool blockActive = !graph || graphIndex == i || graphIndex == FFLFOBlockCount;
    _opType[i] = !blockActive ? 
      FFModulationOpType::Off :
      graph && graphIndex != FFLFOBlockCount ?
      FFModulationOpType::UPAdd :
      topo.NormalizedToListFast<FFModulationOpType>(
      FFLFOParam::OpType,
      FFSelectDualProcBlockParamNormalized<Global>(opTypeNorm[i], voice));
    _steps[i] = topo.NormalizedToDiscreteFast(
      FFLFOParam::Steps,
      FFSelectDualProcBlockParamNormalized<Global>(stepsNorm[i], voice));
    _phase[i] = topo.NormalizedToIdentityFast(
      FFLFOParam::Phase,
      FFSelectDualProcBlockParamNormalized<Global>(phaseNorm[i], voice));
    _waveMode[i] = topo.NormalizedToListFast<FFLFOWaveMode>(
      FFLFOParam::WaveMode,
      FFSelectDualProcBlockParamNormalized<Global>(waveModeNorm[i], voice));
    
    if (_sync)
      _rateHzByBars[i] = topo.NormalizedToBarsFreqFast(FFLFOParam::RateBars,
        FFSelectDualProcBlockParamNormalized<Global>(rateBarsNorm[i], voice), state.input->bpm);

    bool movingGraph = exchangeFromDSP != nullptr && (
      graphIndex == FFLFOBlockCount || 
      _waveMode[i] == FFLFOWaveModeFreeUniRandom || 
      _waveMode[i] == FFLFOWaveModeFreeNormRandom ||
      _waveMode[i] == FFLFOWaveModeFreeUniSmooth ||
      _waveMode[i] == FFLFOWaveModeFreeNormSmooth);

    if (movingGraph)
    {
      _phaseGens[i] = FFTrackingPhaseGenerator(exchangeFromDSP->phases[i]);
      _uniNoiseGens[i].Init(exchangeFromDSP->uniNoiseLastDraw[i], 1, false);
      _normNoiseGens[i].Init(exchangeFromDSP->normNoiseLastDraw[i], 1, false);
      _smoothUniNoiseGens[i].Init(exchangeFromDSP->smoothUniNoiseLastDraw[i], 1, false);
      _smoothNormNoiseGens[i].Init(exchangeFromDSP->smoothNormNoiseLastDraw[i], 1, false);
    }
    else
    {
      if (shouldInit)
      {
        _uniNoiseGens[i].Init(floatSeed, _steps[i] + 1, _waveMode[i] == FFLFOWaveModeFreeUniRandom);
        _normNoiseGens[i].Init(floatSeed, _steps[i] + 1, _waveMode[i] == FFLFOWaveModeFreeNormRandom);
        _smoothUniNoiseGens[i].Init(floatSeed, _steps[i] + 1, _waveMode[i] == FFLFOWaveModeFreeUniSmooth);
        _smoothNormNoiseGens[i].Init(floatSeed, _steps[i] + 1, _waveMode[i] == FFLFOWaveModeFreeNormSmooth);
        _phaseGens[i] = FFTrackingPhaseGenerator(topo.NormalizedToIdentityFast(FFLFOParam::Phase, _phase[i]));
      }
    }
  }
  _globalWasInit = true;
}

template <bool Global>
int
FFLFOProcessor::Process(
  FBModuleProcState& state, bool graph)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& procParams = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gLFO[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vLFO[state.moduleSlot]; });
  auto& dspState = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->dsp.global.gLFO[state.moduleSlot]; },
    [procState, voice, &state]() { return &procState->dsp.voice[voice].vLFO[state.moduleSlot]; });
  auto& outputAll = dspState.outputAll;
  auto& outputRaw = dspState.outputRaw;
  auto const& topo = state.topo->static_->modules[(int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO)];

  if (_type == FFLFOType::Off)
  {
    outputAll.Fill(0.0f);
    for (int i = 0; i < FFLFOBlockCount; i++)
      outputRaw[i].Fill(0.0f);
    return FBFixedBlockSamples;
  }

  if constexpr (!Global)
  {
    if (_finished)
    {
      outputAll.Fill(_smoother.State());
      for (int i = 0; i < FFLFOBlockCount; i++)
        outputRaw[i].Fill(_lastOutputRaw[i]);
      return _graph? 0: FBFixedBlockSamples;
    }
  }

  float sampleRate = state.input->sampleRate;
  auto const& minNormIn = procParams.acc.min;
  auto const& maxNormIn = procParams.acc.max;
  auto const& rateHzNormIn = procParams.acc.rateHz;
  auto const& skewAXAmtNormIn = FFSelectDualProcAccParamNormalized<Global>(procParams.acc.skewAXAmt[0], voice);
  auto const& skewAYAmtNormIn = FFSelectDualProcAccParamNormalized<Global>(procParams.acc.skewAYAmt[0], voice);
  
  FBSArray<float, FBFixedBlockSamples> skewAXAmtNormModulated = {};
  FBSArray<float, FBFixedBlockSamples> skewAYAmtNormModulated = {};
  std::array<FBSArray<float, FBFixedBlockSamples>, FFLFOBlockCount> minNormModulated = {};
  std::array<FBSArray<float, FBFixedBlockSamples>, FFLFOBlockCount> maxNormModulated = {};
  std::array<FBSArray<float, FBFixedBlockSamples>, FFLFOBlockCount> rateHzNormModulated = {};

  skewAXAmtNormIn.CV().CopyTo(skewAXAmtNormModulated);
  skewAYAmtNormIn.CV().CopyTo(skewAYAmtNormModulated);
  for (int i = 0; i < FFLFOBlockCount; i++)
  { 
    FFSelectDualProcAccParamNormalized<Global>(minNormIn[i], voice).CV().CopyTo(minNormModulated[i]);
    FFSelectDualProcAccParamNormalized<Global>(maxNormIn[i], voice).CV().CopyTo(maxNormModulated[i]);
    FFSelectDualProcAccParamNormalized<Global>(rateHzNormIn[i], voice).CV().CopyTo(rateHzNormModulated[i]);
  }

  if constexpr (!Global)
  {
    if (!graph)
    {
      procState->dsp.global.globalUni.processor->ApplyToVoice(state, FFGlobalUniTarget::LFOSkewAX, false, voice, -1, skewAXAmtNormModulated);
      procState->dsp.global.globalUni.processor->ApplyToVoice(state, FFGlobalUniTarget::LFOSkewAY, false, voice, -1, skewAYAmtNormModulated);
      for (int i = 0; i < FFLFOBlockCount; i++)
      {
        procState->dsp.global.globalUni.processor->ApplyToVoice(state, FFGlobalUniTarget::LFOMin, false, voice, -1, minNormModulated[i]);
        procState->dsp.global.globalUni.processor->ApplyToVoice(state, FFGlobalUniTarget::LFOMax, false, voice, -1, maxNormModulated[i]);
        procState->dsp.global.globalUni.processor->ApplyToVoice(state, FFGlobalUniTarget::LFORate, false, voice, -1, rateHzNormModulated[i]);
      }
    }
  }

  FBSArray<float, FBFixedBlockSamples> skewAXAmtPlain;
  FBSArray<float, FBFixedBlockSamples> skewAYAmtPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> minPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> maxPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> rateHzPlain;

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    if(_skewAXMode != FFLFOSkewXMode::Off)
      skewAXAmtPlain.Store(s, topo.NormalizedToIdentityFast(FFLFOParam::SkewAXAmt, skewAXAmtNormModulated.Load(s)));
    if(_skewAYMode != FFLFOSkewYMode::Off)
      skewAYAmtPlain.Store(s, topo.NormalizedToIdentityFast(FFLFOParam::SkewAYAmt, skewAYAmtNormModulated.Load(s)));

    for (int i = 0; i < FFLFOBlockCount; i++)
    {
      if (_opType[i] != FFModulationOpType::Off)
      {
        minPlain[i].Store(s, topo.NormalizedToIdentityFast(FFLFOParam::Min, minNormModulated[i].Load(s)));
        maxPlain[i].Store(s, topo.NormalizedToIdentityFast(FFLFOParam::Max, maxNormModulated[i].Load(s)));
        if (_sync)
          rateHzPlain[i].Store(s, _rateHzByBars[i]);
        else
          rateHzPlain[i].Store(s, topo.NormalizedToLinearFast(FFLFOParam::RateHz, rateHzNormModulated[i].Load(s)));
      }
    }
  }

  if constexpr (Global)
  {
    // Snap to project time.
    if (!_graph && _type == FFLFOType::SnapOrOneShot)
    {
      for (int i = 0; i < FFLFOBlockCount; i++)
      {
        // Rate can go zero by modulation.
        float rate0 = rateHzPlain[i].Get(0);
        if (rate0 > 0.0f)
        {
          float phaseOffset = _phase[i];
          std::int64_t samples0 = (std::int64_t)(sampleRate / rate0);
          std::int64_t phaseSamples = state.input->projectTimeSamples % samples0;
          float newPhase = phaseSamples / (float)samples0 + phaseOffset;
          newPhase -= std::floor(newPhase);
          _phaseGens[i].Reset(newPhase, (int)samples0);
        }
      }
    }
  }

  int s = 0;
  outputAll.Fill(0.0f);
  for (int i = 0; i < FFLFOBlockCount; i++)
    outputRaw[i].Fill(0.0f);
  bool oneShotFinished = !Global && _type == FFLFOType::SnapOrOneShot && _phaseGens[0].CycledOnce();
  for (; s < FBFixedBlockSamples && !oneShotFinished; s += FBSIMDFloatCount)
  {
    for (int i = 0; i < FFLFOBlockCount; i++)
    {
      if (_opType[i] != FFModulationOpType::Off)
      {
        auto incr = rateHzPlain[i].Load(s) / sampleRate;
        auto phase = _phaseGens[i].NextBatch(incr);
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
        case FFLFOWaveModeUniRandom: lfo = _uniNoiseGens[i].NextBatch(phase); break;
        case FFLFOWaveModeNormRandom: lfo = _normNoiseGens[i].NextBatch(phase); break;
        case FFLFOWaveModeFreeUniRandom: lfo = _uniNoiseGens[i].NextBatch(phase); break;
        case FFLFOWaveModeFreeNormRandom: lfo = _normNoiseGens[i].NextBatch(phase); break;
        case FFLFOWaveModeUniSmooth: lfo = _smoothUniNoiseGens[i].NextBatch(phase); break;
        case FFLFOWaveModeNormSmooth: lfo = _smoothNormNoiseGens[i].NextBatch(phase); break;
        case FFLFOWaveModeFreeUniSmooth: lfo = _smoothUniNoiseGens[i].NextBatch(phase); break;
        case FFLFOWaveModeFreeNormSmooth: lfo = _smoothNormNoiseGens[i].NextBatch(phase); break;
        default: lfo = FBToUnipolar(FFCalcTrig(_waveMode[i], phase * 2.0f * FBPi)); break;
        }

        if (i == 0 && _skewAYMode != FFLFOSkewYMode::Off)
        {
          auto skewAYAmt = skewAYAmtPlain.Load(s);
          lfo = SkewY(_skewAYMode, lfo, skewAYAmt);
        }

        if (_steps[i] > 1 && !(FFLFOWaveModeUniRandom <= _waveMode[i] && _waveMode[i] <= FFLFOWaveModeFreeNormSmooth))
        {
          lfo = xsimd::clip(lfo, FBBatch<float>(0.0f), FBBatch<float>(0.9999f));
          lfo = xsimd::floor(lfo * static_cast<float>(_steps[i])) / (_steps[i] - 1.0f);
        }

        auto min = minPlain[i].Load(s);
        auto max = maxPlain[i].Load(s);
        lfo = min + (max - min) * lfo;

        outputRaw[i].Store(s, lfo);
        outputAll.Store(s, FFModulate(_opType[i], lfo, 1.0f, outputAll.Load(s)));
      }

      _lastOutputRaw[i] = outputRaw[i].Get(s + FBSIMDFloatCount - 1);
    }

    _lastOutputAll = outputAll.Get(s + FBSIMDFloatCount - 1);
    oneShotFinished = !Global && _type == FFLFOType::SnapOrOneShot && _phaseGens[0].CycledOnce();
  }

  for (; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    outputAll.Store(s, _lastOutputAll);
    for (int i = 0; i < FFLFOBlockCount; i++)
      outputRaw[i].Store(s, _lastOutputRaw[i]);
  }

  for (s = 0; s < FBFixedBlockSamples; s++)
  {
    float in = outputAll.Get(s);
    if (_firstSample)
    {
      _smoother.State(in);
      _firstSample = false;
    }
    outputAll.Set(s, _smoother.NextScalar(in));
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
  exchangeDSP.boolIsActive = 1;
  exchangeDSP.lengthSamples[FFLFOBlockCount] = 0;
  exchangeDSP.positionSamples[FFLFOBlockCount] = 0;
  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    exchangeDSP.phases[i] = _phaseGens[i].CurrentScalar();
    exchangeDSP.uniNoiseLastDraw[i] = _uniNoiseGens[i].LastDraw();
    exchangeDSP.normNoiseLastDraw[i] = _normNoiseGens[i].LastDraw();
    exchangeDSP.smoothUniNoiseLastDraw[i] = _smoothUniNoiseGens[i].LastDraw();
    exchangeDSP.smoothNormNoiseLastDraw[i] = _smoothNormNoiseGens[i].LastDraw();
    exchangeDSP.lengthSamples[i] = rateHzPlain[i].Last() > 0.0f ? FBFreqToSamples(rateHzPlain[i].Last(), sampleRate) : 0;
    exchangeDSP.lengthSamples[FFLFOBlockCount] = std::max(exchangeDSP.lengthSamples[i], exchangeDSP.lengthSamples[FFLFOBlockCount]);

    // 0: the lines move, so the position indicator stays fixed.
    if (_waveMode[i] == FFLFOWaveModeFreeUniRandom ||
      _waveMode[i] == FFLFOWaveModeFreeNormRandom ||
      _waveMode[i] == FFLFOWaveModeFreeUniSmooth ||
      _waveMode[i] == FFLFOWaveModeFreeNormSmooth)
      exchangeDSP.positionSamples[i] = 0;
    else
      exchangeDSP.positionSamples[i] = _phaseGens[i].PositionSamplesCurrentCycle();
  }

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gLFO[state.moduleSlot]; },
    [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vLFO[state.moduleSlot]; });
  FFSelectDualExchangeState<Global>(exchangeParams.acc.skewAXAmt[0], voice) = skewAXAmtNormModulated.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.skewAYAmt[0], voice) = skewAYAmtNormModulated.Last();
  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    FFSelectDualExchangeState<Global>(exchangeParams.acc.min[i], voice) = minNormModulated[i].Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.max[i], voice) = maxNormModulated[i].Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.rateHz[i], voice) = rateHzNormModulated[i].Last();
  }
   
  return FBFixedBlockSamples;
}

template int FFLFOProcessor::Process<true>(FBModuleProcState&, bool);
template int FFLFOProcessor::Process<false>(FBModuleProcState&, bool);
template void FFLFOProcessor::BeginVoiceOrBlock<true>(FBModuleProcState&, FFLFOExchangeState const*, bool, int, int);
template void FFLFOProcessor::BeginVoiceOrBlock<false>(FBModuleProcState&, FFLFOExchangeState const*, bool, int, int);