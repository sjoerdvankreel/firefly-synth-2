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

template <bool Global>
void
FFLFOProcessor::BeginVoiceOrBlock(
  bool graph, int graphIndex, 
  int graphSampleCount /*todo auto*/, FBModuleProcState& state)
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
  auto const& typeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.type[0], voice);
  auto const& syncNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.sync[0], voice);
  auto const& phaseNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.phase[0], voice);
  auto const& skewXModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewXMode[0], voice);
  auto const& skewYModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewYMode[0], voice);

  _graph = graph;
  _graphSamplesProcessed = 0;
  _graphSampleCount = graphSampleCount;

  _sync = topo.NormalizedToBoolFast(FFLFOParam::Sync, syncNorm); // todo ?
  _type = topo.NormalizedToListFast<FFLFOType>(FFLFOParam::Type, typeNorm);
  _skewXMode = topo.NormalizedToListFast<FFLFOSkewMode>(FFLFOParam::SkewXMode, skewXModeNorm);
  _skewYMode = topo.NormalizedToListFast<FFLFOSkewMode>(FFLFOParam::SkewYMode, skewYModeNorm);

  _rateHzByBars = {};
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

    _phaseGens[i] = FFTimeVectorPhaseGenerator(topo.NormalizedToIdentityFast(FFLFOParam::Phase, phaseNorm));
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

  output.Fill(0.0f);
  if (_type == FFLFOType::Off)
    return FBFixedBlockSamples;

  float sampleRate = state.input->sampleRate;
  auto const& minNorm = procParams.acc.min;
  auto const& maxNorm = procParams.acc.max;
  auto const& rateHzNorm = procParams.acc.rateHz;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> minPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> maxPlain;
  FBSArray2<float, FBFixedBlockSamples, FFLFOBlockCount> rateHzPlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    for (int i = 0; i < FFLFOBlockCount; i++)
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

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    for (int i = 0; i < FFLFOBlockCount; i++)
      if (_opType[i] != FFLFOOpType::Off)
      {
        FBBatch<float> lfo = {};
        auto min = minPlain[i].Load(s);
        auto max = maxPlain[i].Load(s);
        auto incr = rateHzPlain[i].Load(s) / sampleRate;
        auto phase = _phaseGens[i].Next(incr);
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
        output.Add(s, min + (max - min) * lfo);
      }
    output.Store(s, xsimd::clip(output.Load(s), FBBatch<float>(-1.0f), FBBatch<float>(1.0f)));
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
  for (int i = 0; i < FFLFOBlockCount; i++)
    FFSelectDualExchangeState<Global>(exchangeParams.acc.rateHz[i], voice) = FFSelectDualProcAccParamNormalized<Global>(rateHzNorm[i], voice).Last();

  return FBFixedBlockSamples;
}

template int FFLFOProcessor::Process<true>(FBModuleProcState& state);
template int FFLFOProcessor::Process<false>(FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<true>(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<false>(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);