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
FFLFOProcessor::BeginVoiceOrBlock(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gLFO[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vLFO[state.moduleSlot]; });
  auto const& topo = state.topo->static_.modules[(int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO)];

  auto const& phaseNorm = params.block.phase;
  auto const& stepsNorm = params.block.steps;
  auto const& opTypeNorm = params.block.opType;
  auto const& waveModeNorm = params.block.waveMode;
  auto const& typeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.type[0], voice);
  auto const& syncNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.sync[0], voice);
  auto const& seedNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.seed[0], voice);
  auto const& skewXModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewXMode[0], voice);
  auto const& skewYModeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.skewYMode[0], voice);

  _sync = topo.NormalizedToBoolFast(FFLFOParam::Sync, syncNorm); // todo ?
  _seed = topo.NormalizedToDiscreteFast(FFLFOParam::Seed, seedNorm); // todo ?
  _type = topo.NormalizedToListFast<FFLFOType>(FFLFOParam::Type, typeNorm);
  _skewXMode = topo.NormalizedToListFast<FFLFOSkewMode>(FFLFOParam::SkewXMode, skewXModeNorm);
  _skewYMode = topo.NormalizedToListFast<FFLFOSkewMode>(FFLFOParam::SkewYMode, skewYModeNorm);

  for (int i = 0; i < FFLFOBlockCount; i++)
  {
    _steps[i] = topo.NormalizedToDiscreteFast(
      FFLFOParam::Steps,
      FFSelectDualProcBlockParamNormalized<Global>(stepsNorm[i], voice));
    _opType[i] = topo.NormalizedToListFast<FFLFOOpType>(
      FFLFOParam::OpType,
      FFSelectDualProcBlockParamNormalized<Global>(opTypeNorm[i], voice));
    _waveMode[i] = topo.NormalizedToListFast<FFLFOWaveMode>(
      FFLFOParam::WaveMode,
      FFSelectDualProcBlockParamNormalized<Global>(waveModeNorm[i], voice));
    _phaseGens[i] = FFTimeVectorPhaseGenerator(topo.NormalizedToIdentityFast(
      FFLFOParam::Phase,
      FFSelectDualProcBlockParamNormalized<Global>(phaseNorm[i], voice)));
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
  auto const& rateTimeNorm = procParams.acc.rateTime;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> rateTimePlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    for (int i = 0; i < FFEffectBlockCount; i++)
      rateTimePlain[i].Store(s, topo.NormalizedToIdentityFast(FFLFOParam::RateTime,
        FFSelectDualProcAccParamNormalized<Global>(rateTimeNorm[i], voice), s));

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    for (int i = 0; i < FFLFOBlockCount; i++)
      if (_opType[i] != FFLFOOpType::Off)
      {
        auto incr = rateTimePlain[i].Load(s) / sampleRate;
        auto phase = _phaseGens[i].Next(incr);
        auto lfo = FBToUnipolar(xsimd::sin(phase * 2.0f * FBPi));
        output.Add(s, lfo);
      }
    output.Store(s, xsimd::clip(output.Load(s), FBBatch<float>(-1.0f), FBBatch<float>(1.0f)));
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return 0;

  auto& exchangeDSP = *FFSelectDualState<Global>(
    [exchangeToGUI, &state]() { return &exchangeToGUI->global.gLFO[state.moduleSlot]; },
    [exchangeToGUI, &state, voice]() { return &exchangeToGUI->voice[voice].vLFO[state.moduleSlot]; });
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(rateTimePlain[0].Last(), sampleRate);

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gLFO[state.moduleSlot]; },
    [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vLFO[state.moduleSlot]; });
  for (int i = 0; i < FFLFOBlockCount; i++)
    FFSelectDualExchangeState<Global>(exchangeParams.acc.rateTime[i], voice) = FFSelectDualProcAccParamNormalized<Global>(rateTimeNorm[i], voice).Last();

  return FBFixedBlockSamples;
}

template int FFLFOProcessor::Process<true>(FBModuleProcState& state);
template int FFLFOProcessor::Process<false>(FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<true>(FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<false>(FBModuleProcState& state);