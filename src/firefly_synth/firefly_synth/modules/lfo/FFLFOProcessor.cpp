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
  return xsimd::pow(in, (1.0f - amt));
}

static inline FBBatch<float>
SkewExpBipolar(FBBatch<float> in, FBBatch<float> amt)
{
  auto bp = FBToBipolar(in);
  return FBToUnipolar(xsimd::sign(bp) * xsimd::pow(xsimd::abs(bp), (1.0f - amt)));
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
  bool graph, int graphIndex, 
  int graphSampleCount /*todo auto*/, FBModuleProcState& state)
{
  _smoother = {};
  _rateHzByBars = {};
  _smoothSamples = 0;
  _firstSample = true;

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
  auto const& typeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.type[0], voice);
  auto const& syncNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.sync[0], voice);
  auto const& phaseBNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.phaseB[0], voice);
  auto const& skewAXModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewAXMode[0], voice);
  auto const& skewAYModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewAYMode[0], voice);
  auto const& smoothBarsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.smoothBars[0], voice);
  auto const& smoothTimeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.smoothTime[0], voice);

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

  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    bool blockActive = !graph || graphIndex == i || graphIndex == FFLFOBlockCount;
    _opType[i] = !blockActive ? FFLFOOpType::Off : topo.NormalizedToListFast<FFLFOOpType>(
      FFLFOParam::OpType,
      FFSelectDualProcBlockParamNormalized<Global>(opTypeNorm[i], voice));
    _steps[i] = topo.NormalizedToDiscreteFast(
      FFLFOParam::Steps,
      FFSelectDualProcBlockParamNormalized<Global>(stepsNorm[i], voice));
    _waveMode[i] = topo.NormalizedToListFast<FFLFOWaveMode>(
      FFLFOParam::WaveMode,
      FFSelectDualProcBlockParamNormalized<Global>(waveModeNorm[i], voice));

    if(i == 1)
      _phaseGens[i] = FFTimeVectorPhaseGenerator(topo.NormalizedToIdentityFast(FFLFOParam::PhaseB, phaseBNorm));
    else
      _phaseGens[i] = FFTimeVectorPhaseGenerator(0.0f);

    if (_sync)
      _rateHzByBars[i] = topo.NormalizedToBarsFreqFast(FFLFOParam::RateBars,
        FFSelectDualProcBlockParamNormalized<Global>(rateBarsNorm[i], voice), state.input->bpm);
  }
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

  // TODO smoother for 1 shot
  output.Fill(0.0f);
  if (_type == FFLFOType::Off)
    return FBFixedBlockSamples;

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

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    for (int i = 0; i < FFLFOBlockCount; i++)
    {
      if (_opType[i] != FFLFOOpType::Off)
      {
        auto incr = rateHzPlain[i].Load(s) / sampleRate;
        auto phase = _phaseGens[i].Next(incr);

        if (i == 0 && _skewAXMode != FFLFOSkewXMode::Off)
        {
          auto skewAXAmt = skewAXAmtPlain.Load(s);
          phase = SkewX(_skewAXMode, phase, skewAXAmt);
        }

        FBBatch<float> lfo = {};
        switch (_waveMode[i])
        {
        case FFLFOWaveModeSaw: lfo = phase; break;
        case FFLFOWaveModeTri: break;
        case FFLFOWaveModeSqr: break;
        case FFLFOWaveModeRandom: break;
        case FFLFOWaveModeFreeRandom: break;
        case FFLFOWaveModeSmooth: break;
        case FFLFOWaveModeFreeSmooth: break;
        default: lfo = FBToUnipolar(FFCalcTrig(_waveMode[i], phase * 2.0f * FBPi)); break;
        }

        if (i == 0 && _skewAYMode != FFLFOSkewYMode::Off)
        {
          auto skewAYAmt = skewAYAmtPlain.Load(s);
          lfo = SkewY(_skewAYMode, lfo, skewAYAmt);
        }

        // todo not always
        if (_steps[i] > 1)
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
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float in = output.Get(s);
    if (_firstSample)
    {
      _smoother.State(in);
      _firstSample = false;
    }
    output.Set(s, _smoother.Next(in));
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
  exchangeDSP.lengthSamples = FBFreqToSamples(rateHzPlain[0].Last(), sampleRate);

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

template int FFLFOProcessor::Process<true>(FBModuleProcState& state);
template int FFLFOProcessor::Process<false>(FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<true>(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<false>(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);