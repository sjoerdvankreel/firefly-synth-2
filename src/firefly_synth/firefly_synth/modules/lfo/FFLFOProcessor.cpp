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
    _phase[i] = topo.NormalizedToIdentityFast(
      FFLFOParam::Phase,
      FFSelectDualProcBlockParamNormalized<Global>(phaseNorm[i], voice));
    _opType[i] = topo.NormalizedToListFast<FFLFOOpType>(
      FFLFOParam::OpType,
      FFSelectDualProcBlockParamNormalized<Global>(opTypeNorm[i], voice));
    _waveMode[i] = topo.NormalizedToListFast<FFLFOWaveMode>(
      FFLFOParam::WaveMode,
      FFSelectDualProcBlockParamNormalized<Global>(waveModeNorm[i], voice));
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

  if (_type == FFLFOType::Off)
  {
    output.Fill(0.0f);
    return FBFixedBlockSamples;
  }

  float sampleRate = state.input->sampleRate;
  auto const& rateNorm = procParams.acc.rateTime;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> ratePlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    for (int i = 0; i < FFEffectBlockCount; i++)
      ratePlain[i].Store(s, topo.NormalizedToIdentityFast(FFLFOParam::RateTime,
        FFSelectDualProcAccParamNormalized<Global>(rateNorm[i], voice), s));

#if 0

  for (int i = 0; i < FFLFOBlockCount; i++)
    switch (_kind[i])
    {
    case FFEffectKind::Fold:
      ProcessFold(i, oversampled, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::Clip:
      ProcessClip(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::Skew:
      ProcessSkew(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::StVar:
      ProcessStVar<Global>(i, oversampledRate, oversampled, trackingKeyPlain, stVarResPlain, stVarFreqPlain, stVarGainPlain, stVarKeyTrkPlain);
      break;
    case FFEffectKind::Comb:
      ProcessComb<Global, true, true>(i, oversampledRate, oversampled, trackingKeyPlain, combKeyTrkPlain, combResMinPlain, combResPlusPlain, combFreqMinPlain, combFreqPlusPlain);
      break;
    case FFEffectKind::CombPlus:
      ProcessComb<Global, true, false>(i, oversampledRate, oversampled, trackingKeyPlain, combKeyTrkPlain, combResMinPlain, combResPlusPlain, combFreqMinPlain, combFreqPlusPlain);
      break;
    case FFEffectKind::CombMin:
      ProcessComb<Global, false, true>(i, oversampledRate, oversampled, trackingKeyPlain, combKeyTrkPlain, combResMinPlain, combResPlusPlain, combFreqMinPlain, combFreqPlusPlain);
      break;
    default:
      break;
    }

  if (_oversampleTimes == 1)
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        output[c].Store(s, oversampled[c].Load(s));
  else
  {
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < FFEffectFixedBlockOversamples; s++)
        oversampledBlock.setSample(c, s, oversampled[c].Get(s));
    float* audioOut[2] = {};
    audioOut[0] = output[0].Ptr(0);
    audioOut[1] = output[1].Ptr(0);
    AudioBlock<float> outputBlock(audioOut, 2, 0, FBFixedBlockSamples);
    _oversampler.processSamplesDown(outputBlock);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    _graphSamplesProcessed += FBFixedBlockSamples;
    return std::clamp(_graphSampleCount - _graphSamplesProcessed, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = *FFSelectDualState<Global>(
    [exchangeToGUI, &state]() { return &exchangeToGUI->global.gEffect[state.moduleSlot]; },
    [exchangeToGUI, &state, voice]() { return &exchangeToGUI->voice[voice].vEffect[state.moduleSlot]; });
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBTimeToSamples(FFEffectPlotLengthSeconds, sampleRate);

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gEffect[state.moduleSlot]; },
    [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vEffect[state.moduleSlot]; });
  FFSelectDualExchangeState<Global>(exchangeParams.acc.trackingKey[0], voice) = trackingKeyNorm.Last();
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    FFSelectDualExchangeState<Global>(exchangeParams.acc.distAmt[i], voice) = FFSelectDualProcAccParamNormalized<Global>(distAmtNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.distMix[i], voice) = FFSelectDualProcAccParamNormalized<Global>(distMixNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.distBias[i], voice) = FFSelectDualProcAccParamNormalized<Global>(distBiasNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.distDrive[i], voice) = FFSelectDualProcAccParamNormalized<Global>(distDriveNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarRes[i], voice) = FFSelectDualProcAccParamNormalized<Global>(stVarResNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarFreq[i], voice) = FFSelectDualProcAccParamNormalized<Global>(stVarFreqNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarGain[i], voice) = FFSelectDualProcAccParamNormalized<Global>(stVarGainNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarKeyTrk[i], voice) = FFSelectDualProcAccParamNormalized<Global>(stVarKeyTrkNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combKeyTrk[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combKeyTrkNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combResMin[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combResMinNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combResPlus[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combResPlusNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combFreqMin[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combFreqMinNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combFreqPlus[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combFreqPlusNorm[i], voice).Last();
  }

#endif
  return FBFixedBlockSamples;
}

template int FFLFOProcessor::Process<true>(FBModuleProcState& state);
template int FFLFOProcessor::Process<false>(FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<true>(FBModuleProcState& state);
template void FFLFOProcessor::BeginVoiceOrBlock<false>(FBModuleProcState& state);