#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

#include <xsimd/xsimd.hpp>

using namespace juce::dsp;

static inline FFStateVariableFilterMode
EffectKindToSVFMode(FFEffectKind kind)
{
  switch (kind)
  {
  case FFEffectKind::LPF: return FFStateVariableFilterMode::LPF;
  case FFEffectKind::BPF: return FFStateVariableFilterMode::BPF;
  case FFEffectKind::HPF: return FFStateVariableFilterMode::HPF;
  case FFEffectKind::BSF: return FFStateVariableFilterMode::BSF;
  case FFEffectKind::APF: return FFStateVariableFilterMode::APF;
  case FFEffectKind::PEQ: return FFStateVariableFilterMode::PEQ;
  case FFEffectKind::BLL: return FFStateVariableFilterMode::BLL;
  case FFEffectKind::LSH: return FFStateVariableFilterMode::LSH;
  case FFEffectKind::HSH: return FFStateVariableFilterMode::HSH;
  default: FB_ASSERT(false); return {};
  }
}

static inline FBBatch<float>
FoldBack(FBBatch<float> in)
{
  FBSArray<float, FBSIMDFloatCount> out;
  out.Store(0, in);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float x = std::clamp(out.Get(i), -32.0f, 32.0f);
    while (true)
    {
      if (x > 1.0f) x -= 2.0f * (x - 1.0f);
      else if (x < -1.0f) x += 2.0f * (-x - 1.0f);
      else break;
    }
    out.Set(i, x);
  }
  return out.Load(0);
}

FFEffectProcessor::
FFEffectProcessor() :
_oversampler(
  2, FFEffectOversampleFactor,
  Oversampling<float>::filterHalfBandPolyphaseIIR, false, false)
{
  _oversampler.initProcessing(FBFixedBlockSamples);
}

template <bool Global>
FBBatch<float> 
FFEffectProcessor::NextBasePitchBatch(int pos)
{
  if constexpr (Global)
    return _basePitchSmoother.NextBatch(_basePitch.Load(pos));
  else
    return _basePitch.Load(pos);
}

void 
FFEffectProcessor::ReleaseOnDemandBuffers(
  FBRuntimeTopo const*, FBProcStateContainer* state)
{
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    _combFilters[i].ReleaseBuffers(state->MemoryPool());
    _compRMSTotal[i] = 0.0f;
    _compRMSWindowsPos[i] = 0;
    _compRMSWindows[i] = std::vector<float>();
  }
}

template <bool Global>
void 
FFEffectProcessor::AllocOnDemandBuffers(
  FBRuntimeTopo const* topo, FBProcStateContainer* state,
  int moduleSlot, bool graph, float sampleRate)
{
  // for graphing we are toying with the parameters to fit the plots
  // also engine state may not match the main state
  // just make it easy and allocate it all, we'll release soon and sample rate is low anyway

  auto* procState = state->RawAs<FFProcState>();
  auto const& params = *FFSelectDualState<Global>(
    [procState, moduleSlot]() { return &procState->param.global.gEffect[moduleSlot]; },
    [procState, moduleSlot]() { return &procState->param.voice.vEffect[moduleSlot]; });
  auto const& kindNorm = params.block.kind;
  float onNorm = FFSelectDualProcBlockParamNormalizedGlobal<Global>(params.block.on[0]);
  auto const& moduleTopo = topo->static_->modules[(int)(Global ? FFModuleType::GEffect : FFModuleType::VEffect)];

  if (!graph && !moduleTopo.NormalizedToBoolFast(FFEffectParam::On, onNorm))
    return;

  float graphFilterFreqMultiplier = FFGraphFilterFreqMultiplier(graph, sampleRate, FFMaxCombFilterFreq);
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    auto kind = moduleTopo.NormalizedToListFast<FFEffectKind>(FFEffectParam::Kind,
      FFSelectDualProcBlockParamNormalizedGlobal<Global>(kindNorm[i]));
    if (graph || FFEffectKindIsComb(kind))
      _combFilters[i].AllocBuffers(state->MemoryPool(), sampleRate * FFEffectOversampleTimes, FFMinCombFilterFreq * graphFilterFreqMultiplier); 
    if (kind == FFEffectKind::Compressor)
    {
      float rmsWindowSize = moduleTopo.NormalizedToLinearFast(FFEffectParam::CompRMSSize,
        FFSelectDualProcBlockParamNormalizedGlobal<Global>(params.block.compRMSSize[i]));
      int samples = (int)std::ceil(rmsWindowSize * sampleRate);
      if ((int)_compRMSWindows[i].size() != samples)
      {
        _compRMSTotal[i] = 0.0f;
        _compRMSWindowsPos[i] = 0;
        _compRMSWindows[i] = std::vector<float>(samples, 0.0f);
      }
    }
  }
}

template <bool Global>
void
FFEffectProcessor::BeginVoiceOrBlock(
  FBModuleProcState& state, 
  bool graph, bool detailGraphs,
  int graphIndex, int graphSampleCount)
{
  auto* procState = state.ProcAs<FFProcState>();
  float sampleRate = state.input->sampleRate;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gEffect[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vEffect[state.moduleSlot]; });
  auto const& topo = state.topo->static_->modules[(int)(Global? FFModuleType::GEffect: FFModuleType::VEffect)];
  
  auto const& kindNorm = params.block.kind;
  auto const& clipModeNorm = params.block.clipMode;
  auto const& foldModeNorm = params.block.foldMode;
  auto const& skewModeNorm = params.block.skewMode;
  auto const& filterModeNorm = params.block.filterMode;

  auto const& compKneeNorm = params.block.compKnee;
  auto const& compRatioNorm = params.block.compRatio;
  auto const& compAttackNorm = params.block.compAttack;
  auto const& compReleaseNorm = params.block.compRelease;
  auto const& compThresholdNorm = params.block.compThreshold;
  auto const& compModeNorm = params.block.compMode;
  auto const& compRMSSizeNorm = params.block.compRMSSize;
  // TODO auto const& compVSideOrGSideNorm = params.block.compVSideOrGSide;

  float onNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.on[0], voice);
  float oversampleNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.oversample[0], voice);
  float lastKeySmoothTimeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.lastKeySmoothTime[0], voice);

  _graph = graph;
  _graphSamplesProcessed = 0;
  _graphSampleCount = graphSampleCount;
  _graphCombFilterFreqMultiplier = FFGraphFilterFreqMultiplier(graph, state.input->sampleRate, FFMaxCombFilterFreq);
  _graphStVarFilterFreqMultiplier = FFGraphFilterFreqMultiplier(graph, state.input->sampleRate, FFMaxStateVariableFilterFreq);

  _on = topo.NormalizedToBoolFast(FFEffectParam::On, onNorm);
  bool oversample = topo.NormalizedToBoolFast(FFEffectParam::Oversample, oversampleNorm);
  _oversampleTimes = !graph && oversample ? FFEffectOversampleTimes : 1;
  int smoothSamples = topo.NormalizedToLinearTimeSamplesFast(
    FFEffectParam::LastKeySmoothTime, lastKeySmoothTimeNorm, state.input->sampleRate);
  _basePitchSmoother.SetCoeffs(smoothSamples);

  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    bool blockActive = !graph || graphIndex == i || !detailGraphs;
    _kind[i] = !blockActive? FFEffectKind::Off: topo.NormalizedToListFast<FFEffectKind>(
      FFEffectParam::Kind, 
      FFSelectDualProcBlockParamNormalized<Global>(kindNorm[i], voice));
    _clipMode[i] = topo.NormalizedToListFast<FFEffectClipMode>(
      FFEffectParam::ClipMode, 
      FFSelectDualProcBlockParamNormalized<Global>(clipModeNorm[i], voice));
    _foldMode[i] = topo.NormalizedToListFast<FFEffectFoldMode>(
      FFEffectParam::FoldMode, 
      FFSelectDualProcBlockParamNormalized<Global>(foldModeNorm[i], voice));
    _skewMode[i] = topo.NormalizedToListFast<FFEffectSkewMode>(
      FFEffectParam::SkewMode, 
      FFSelectDualProcBlockParamNormalized<Global>(skewModeNorm[i], voice));
    _filterMode[i] = topo.NormalizedToListFast<FFEffectFilterMode>(
      FFEffectParam::FilterMode,
      FFSelectDualProcBlockParamNormalized<Global>(filterModeNorm[i], voice));

    _compKneeDb[i] = topo.NormalizedToLinearFast(FFEffectParam::CompKnee,
      FFSelectDualProcBlockParamNormalized<Global>(compKneeNorm[i], voice));
    _compRatio[i] = topo.NormalizedToLinearFast(FFEffectParam::CompRatio,
      FFSelectDualProcBlockParamNormalized<Global>(compRatioNorm[i], voice));
    _compMode[i] = topo.NormalizedToListFast<FFEffectCompMode>(
      FFEffectParam::CompMode,
      FFSelectDualProcBlockParamNormalized<Global>(compModeNorm[i], voice));
    _compRMSSize[i] = topo.NormalizedToLinearFast(
      FFEffectParam::CompRMSSize,
      FFSelectDualProcBlockParamNormalized<Global>(compRMSSizeNorm[i], voice));
    _compThresholdDb[i] = topo.NormalizedToLinearFast(FFEffectParam::CompThreshold,
      FFSelectDualProcBlockParamNormalized<Global>(compThresholdNorm[i], voice));
    _compThresholdDb[i] = 20.0f * std::log10(_compThresholdDb[i]);

    float compAttackTime = topo.NormalizedToLinearFast(
      FFEffectParam::CompAttack,
      FFSelectDualProcBlockParamNormalized<Global>(compAttackNorm[i], voice));
    float compReleaseTime = topo.NormalizedToLinearFast(
      FFEffectParam::CompRelease,
      FFSelectDualProcBlockParamNormalized<Global>(compReleaseNorm[i], voice));
    if (_graph)
    {
      compAttackTime = 0.0f;
      compReleaseTime = 0.0f;
    }
    _compEnvCoeffAttack[i] = std::exp(-1.0f / (compAttackTime * sampleRate * _oversampleTimes));
    _compEnvCoeffRelease[i] = std::exp(-1.0f / (compReleaseTime * sampleRate * _oversampleTimes));

#if false // TODO
    if constexpr(Global)
      _gCompSide[i] = topo.NormalizedToListFast<FFGEffectCompSide>(
        FFEffectParam::CompVSideOrGSide,
        FFSelectDualProcBlockParamNormalized<Global>(compVSideOrGSideNorm[i], voice));
    else
      _vCompSide[i] = topo.NormalizedToListFast<FFVEffectCompSide>(
        FFEffectParam::CompVSideOrGSide,
        FFSelectDualProcBlockParamNormalized<Global>(compVSideOrGSideNorm[i], voice));
#endif 

    if constexpr(Global)
      if (!graph)
        continue;
    _combFilters[i].Reset();
    _stVarFilters[i].Reset();
  }
}

template <bool Global>
int
FFEffectProcessor::Process(
  FBModuleProcState& state,
  FFEffectExchangeState const* exchangeFromDSP,
  bool graph)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& procParams = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gEffect[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vEffect[state.moduleSlot]; });
  auto& dspState = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->dsp.global.gEffect[state.moduleSlot]; },
    [procState, voice, &state]() { return &procState->dsp.voice[voice].vEffect[state.moduleSlot]; });
  auto& output = dspState.output;
  auto const& input = dspState.input;
  auto const& topo = state.topo->static_->modules[(int)(Global ? FFModuleType::GEffect : FFModuleType::VEffect)];

  if (!_on)
  {
    input.CopyTo(output);
    return FBFixedBlockSamples;
  }

  float sampleRate = state.input->sampleRate;
  auto const& distAmtNorm = procParams.acc.distAmt;
  auto const& distMixNorm = procParams.acc.distMix;
  auto const& distBiasNorm = procParams.acc.distBias;
  auto const& stVarResNorm = procParams.acc.stVarRes;
  auto const& stVarGainNorm = procParams.acc.stVarGain;
  auto const& stVarKeyTrkNorm = procParams.acc.stVarKeyTrk;
  auto const& combKeyTrkNorm = procParams.acc.combKeyTrk;
  auto const& combResMinNorm = procParams.acc.combResMin;
  auto const& combResPlusNorm = procParams.acc.combResPlus;
  auto const& trackingKeyNorm = FFSelectDualProcAccParamNormalized<Global>(procParams.acc.trackingKey[0], voice);

  auto const& lfoOutput = *FFSelectDualState<Global>(
    [procState]() { return &procState->dsp.global.gLFO; },
    [procState, voice]() { return &procState->dsp.voice[voice].vLFO; });

  auto const& envAmtNorm = procParams.acc.envAmt;
  auto const& lfoAmtNorm = procParams.acc.lfoAmt;
  auto const& distDriveNormIn = procParams.acc.distDrive;
  auto const& stVarFreqFreqNormIn = procParams.acc.stVarFreqFreq;
  auto const& stVarPitchCoarseNormIn = procParams.acc.stVarPitchCoarse;
  auto const& combFreqFreqMinNormIn = procParams.acc.combFreqFreqMin;
  auto const& combPitchCoarseMinNormIn = procParams.acc.combPitchCoarseMin;
  auto const& combFreqFreqPlusNormIn = procParams.acc.combFreqFreqPlus;
  auto const& combPitchCoarsePlusNormIn = procParams.acc.combPitchCoarsePlus;

  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarFreqFreqPlain;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarPitchCoarsePlain;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combFreqFreqMinPlain;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combPitchCoarseMinPlain;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combFreqFreqPlusPlain;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combPitchCoarsePlusPlain;

  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> distDriveNormModulated;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarFreqFreqNormModulated;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarPitchCoarseNormModulated;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combFreqFreqMinNormModulated;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combPitchCoarseMinNormModulated;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combFreqFreqPlusNormModulated;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combPitchCoarsePlusNormModulated;

  FBSArray<float, FFEffectFixedBlockOversamples> trackingKeyPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> distAmtPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> distMixPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> distBiasPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> distDrivePlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> stVarResPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> stVarRealFreqPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> stVarGainPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> stVarKeyTrkPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> combKeyTrkPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> combResMinPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> combResPlusPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> combRealFreqMinPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> combRealFreqPlusPlain;

  if (graph)
  {
    if (exchangeFromDSP != nullptr)
      _basePitch.Fill(FBBatch<float>(exchangeFromDSP->basePitch));
    else
      _basePitch.Fill(FBBatch<float>(60.0f));
  }
  else
  {
    if constexpr (Global)
    {
      // raw because we provide our own smoothing filter
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        _basePitch.Store(s, procState->dsp.global.master.outputLastKeyPitchTunedRaw.Load(s) * 127.0f);
    }
    else
    {
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        _basePitch.Store(s, procState->dsp.voice[voice].voiceModule.pitch.Load(s));
    }
  }
  
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    trackingKeyPlain.Store(s, topo.NormalizedToLinearFast(FFEffectParam::TrackingKey, trackingKeyNorm, s));
    for (int i = 0; i < FFEffectBlockCount; i++)
    {
      if (FFEffectKindIsSVF(_kind[i]))
      {
        stVarResPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::StVarRes,
          FFSelectDualProcAccParamNormalized<Global>(stVarResNorm[i], voice), s));
        stVarGainPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::StVarGain,
          FFSelectDualProcAccParamNormalized<Global>(stVarGainNorm[i], voice), s));
        stVarKeyTrkPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::StVarKeyTrak,
          FFSelectDualProcAccParamNormalized<Global>(stVarKeyTrkNorm[i], voice), s));

        if(_filterMode[i] == FFEffectFilterMode::Freq)
          stVarFreqFreqNormModulated[i].Store(s, FFSelectDualProcAccParamNormalized<Global>(stVarFreqFreqNormIn[i], voice).CV().Load(s));
        else
          stVarPitchCoarseNormModulated[i].Store(s, FFSelectDualProcAccParamNormalized<Global>(stVarPitchCoarseNormIn[i], voice).CV().Load(s));

        if (!_graph)
        {
          if (_filterMode[i] == FFEffectFilterMode::Freq)
            stVarFreqFreqNormModulated[i].Store(s,
              FFModulate(FFModulationOpType::BPStack, lfoOutput[i].outputAll.Load(s),
                FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                FFSelectDualProcAccParamNormalized<Global>(stVarFreqFreqNormIn[i], voice).CV().Load(s)));
          else
            stVarPitchCoarseNormModulated[i].Store(s,
              FFModulate(FFModulationOpType::BPStack, lfoOutput[i].outputAll.Load(s),
                FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                FFSelectDualProcAccParamNormalized<Global>(stVarPitchCoarseNormIn[i], voice).CV().Load(s)));

          if constexpr (!Global)
          {
            int uniTargetParamBase = (int)FFGlobalUniTarget::VFXParamA;
            FFGlobalUniTarget uniTargetParam = (FFGlobalUniTarget)(uniTargetParamBase + i);
            if (_filterMode[i] == FFEffectFilterMode::Freq)
            {
              stVarFreqFreqNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::UPStack, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                  stVarFreqFreqNormModulated[i].Load(s)));
              procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, stVarFreqFreqNormModulated[i]);
            }
            else
            {
              stVarPitchCoarseNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::UPStack, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                  stVarPitchCoarseNormModulated[i].Load(s)));
              procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, stVarPitchCoarseNormModulated[i]);
            }
          }
        }

        FBBatch<float> realFreqPlain;
        auto trkk = trackingKeyPlain.Load(s);
        auto ktrk = stVarKeyTrkPlain[i].Load(s);
        if (_filterMode[i] == FFEffectFilterMode::Freq)
        {
          auto freqFreqPlain = topo.NormalizedToLog2Fast(FFEffectParam::StVarFreqFreq, stVarFreqFreqNormModulated[i].Load(s));
          freqFreqPlain = FFMultiplyClamp(freqFreqPlain, FFKeyboardTrackingMultiplier(NextBasePitchBatch<Global>(s), trkk, ktrk),
            FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
          stVarFreqFreqPlain[i].Store(s, freqFreqPlain);
          realFreqPlain = freqFreqPlain;
        }
        else
        {
          auto basePitch = _filterMode[i] == FFEffectFilterMode::Track? NextBasePitchBatch<Global>(s): FBBatch<float>(0.0f);
          auto coarsePlain = topo.NormalizedToLinearFast(FFEffectParam::StVarPitchCoarse, stVarPitchCoarseNormModulated[i].Load(s));
          auto trackAgainst = _filterMode[i] == FFEffectFilterMode::Track ? basePitch : coarsePlain;
          coarsePlain += (trackAgainst - (trkk + 60.0f)) * ktrk;
          coarsePlain = xsimd::clip(coarsePlain, FBBatch<float>(-FFModCoarseSemis), FBBatch<float>(FFModCoarseSemis));
          stVarPitchCoarsePlain[i].Store(s, coarsePlain);
          realFreqPlain = xsimd::clip(FBPitchToFreq(basePitch + coarsePlain), FBBatch<float>(FFMinStateVariableFilterFreq), FBBatch<float>(FFMaxStateVariableFilterFreq));
        }

        if (_graph)
        {
          realFreqPlain *= _graphStVarFilterFreqMultiplier;
          FBBatch<float> clampMin = 1.01f * _graphStVarFilterFreqMultiplier * FFMinStateVariableFilterFreq;
          FBBatch<float> clampMax = 0.99f * _graphStVarFilterFreqMultiplier * FFMaxStateVariableFilterFreq;
          realFreqPlain = xsimd::clip(realFreqPlain, clampMin, clampMax);
        }

        stVarRealFreqPlain[i].Store(s, realFreqPlain);
      }
      else if (FFEffectKindIsComb(_kind[i]))
      {
        combKeyTrkPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombKeyTrk,
          FFSelectDualProcAccParamNormalized<Global>(combKeyTrkNorm[i], voice), s));

        if (_kind[i] == FFEffectKind::Comb || _kind[i] == FFEffectKind::CombMin)
        {
          combResMinPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombResMin,
            FFSelectDualProcAccParamNormalized<Global>(combResMinNorm[i], voice), s));

          if (_filterMode[i] == FFEffectFilterMode::Freq)
            combFreqFreqMinNormModulated[i].Store(s,
              FFSelectDualProcAccParamNormalized<Global>(combFreqFreqMinNormIn[i], voice).CV().Load(s));
          else
            combPitchCoarseMinNormModulated[i].Store(s, 
              FFSelectDualProcAccParamNormalized<Global>(combPitchCoarseMinNormIn[i], voice).CV().Load(s));

          if (!_graph)
          {
            if (_filterMode[i] == FFEffectFilterMode::Freq)
              combFreqFreqMinNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::BPStack, lfoOutput[i].outputAll.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(combFreqFreqMinNormIn[i], voice).CV().Load(s)));
            else
              combPitchCoarseMinNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::BPStack, lfoOutput[i].outputAll.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(combPitchCoarseMinNormIn[i], voice).CV().Load(s)));           

            if constexpr (!Global)
            {
              int uniTargetParamBase = (int)FFGlobalUniTarget::VFXParamA;
              FFGlobalUniTarget uniTargetParam = (FFGlobalUniTarget)(uniTargetParamBase + i);
              if (_filterMode[i] == FFEffectFilterMode::Freq)
              {
                combFreqFreqMinNormModulated[i].Store(s,
                  FFModulate(FFModulationOpType::UPStack, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                    FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                    combFreqFreqMinNormModulated[i].Load(s)));
                procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, combFreqFreqMinNormModulated[i]);
              }
              else
              {
                combPitchCoarseMinNormModulated[i].Store(s,
                  FFModulate(FFModulationOpType::UPStack, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                    FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                    combPitchCoarseMinNormModulated[i].Load(s)));
                procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, combPitchCoarseMinNormModulated[i]);
              }
            }
          }        

          FBBatch<float> realFreqPlain;
          auto trkk = trackingKeyPlain.Load(s);
          auto ktrk = combKeyTrkPlain[i].Load(s);
          if (_filterMode[i] == FFEffectFilterMode::Freq)
          {
            auto freqFreqPlain = topo.NormalizedToLog2Fast(FFEffectParam::CombFreqFreqMin, combFreqFreqMinNormModulated[i].Load(s));
            freqFreqPlain = FFMultiplyClamp(freqFreqPlain, FFKeyboardTrackingMultiplier(NextBasePitchBatch<Global>(s), trkk, ktrk),
              FFMinCombFilterFreq, FFMaxCombFilterFreq);
            combFreqFreqMinPlain[i].Store(s, freqFreqPlain);
            realFreqPlain = freqFreqPlain;
          }
          else
          {
            auto basePitch = _filterMode[i] == FFEffectFilterMode::Track ? NextBasePitchBatch<Global>(s) : FBBatch<float>(0.0f);
            auto coarsePlain = topo.NormalizedToLinearFast(FFEffectParam::CombPitchCoarseMin, combPitchCoarseMinNormModulated[i].Load(s));
            auto trackAgainst = _filterMode[i] == FFEffectFilterMode::Track ? basePitch : coarsePlain;
            coarsePlain += (trackAgainst - (trkk + 60.0f)) * ktrk;
            coarsePlain = xsimd::clip(coarsePlain, FBBatch<float>(-FFModCoarseSemis), FBBatch<float>(FFModCoarseSemis));
            combPitchCoarseMinPlain[i].Store(s, coarsePlain);
            realFreqPlain = xsimd::clip(FBPitchToFreq(basePitch + coarsePlain), FBBatch<float>(FFMinCombFilterFreq), FBBatch<float>(FFMaxCombFilterFreq));
          }

          if (_graph)
          {
            realFreqPlain *= _graphCombFilterFreqMultiplier;
            FBBatch<float> clampMin = 1.01f * _graphCombFilterFreqMultiplier * FFMinCombFilterFreq;
            FBBatch<float> clampMax = 0.99f * _graphCombFilterFreqMultiplier * FFMaxCombFilterFreq;
            realFreqPlain = xsimd::clip(realFreqPlain, clampMin, clampMax);
          }

          combRealFreqMinPlain[i].Store(s, realFreqPlain);
        }
        if (_kind[i] == FFEffectKind::Comb || _kind[i] == FFEffectKind::CombPlus)
        {
          combResPlusPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombResPlus,
            FFSelectDualProcAccParamNormalized<Global>(combResPlusNorm[i], voice), s));
         
          if (_filterMode[i] == FFEffectFilterMode::Freq)
            combFreqFreqPlusNormModulated[i].Store(s,
              FFSelectDualProcAccParamNormalized<Global>(combFreqFreqPlusNormIn[i], voice).CV().Load(s));
          else
            combPitchCoarsePlusNormModulated[i].Store(s,
              FFSelectDualProcAccParamNormalized<Global>(combPitchCoarsePlusNormIn[i], voice).CV().Load(s));

          if (!_graph)
          {
            if (_filterMode[i] == FFEffectFilterMode::Freq)
              combFreqFreqPlusNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::BPStack, lfoOutput[i].outputAll.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(combFreqFreqPlusNormIn[i], voice).CV().Load(s)));
            else
              combPitchCoarsePlusNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::BPStack, lfoOutput[i].outputAll.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(combPitchCoarsePlusNormIn[i], voice).CV().Load(s)));

            if constexpr (!Global)
            {
              int uniTargetParamBase = (int)FFGlobalUniTarget::VFXParamA;
              FFGlobalUniTarget uniTargetParam = (FFGlobalUniTarget)(uniTargetParamBase + i);
              if (_filterMode[i] == FFEffectFilterMode::Freq)
              {
                combFreqFreqPlusNormModulated[i].Store(s,
                  FFModulate(FFModulationOpType::UPStack, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                    FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                    combFreqFreqPlusNormModulated[i].Load(s)));
                procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, combFreqFreqPlusNormModulated[i]);
              }
              else
              {
                combPitchCoarsePlusNormModulated[i].Store(s,
                  FFModulate(FFModulationOpType::UPStack, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                    FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                    combPitchCoarsePlusNormModulated[i].Load(s)));
                procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, combPitchCoarsePlusNormModulated[i]);
              }
            }
          }

          FBBatch<float> realFreqPlain;
          auto trkk = trackingKeyPlain.Load(s);
          auto ktrk = combKeyTrkPlain[i].Load(s);
          if (_filterMode[i] == FFEffectFilterMode::Freq)
          {
            auto freqFreqPlain = topo.NormalizedToLog2Fast(FFEffectParam::CombFreqFreqPlus, combFreqFreqPlusNormModulated[i].Load(s));
            freqFreqPlain = FFMultiplyClamp(freqFreqPlain, FFKeyboardTrackingMultiplier(NextBasePitchBatch<Global>(s), trkk, ktrk),
              FFMinCombFilterFreq, FFMaxCombFilterFreq);
            combFreqFreqPlusPlain[i].Store(s, freqFreqPlain);
            realFreqPlain = freqFreqPlain;
          }
          else
          {
            auto basePitch = _filterMode[i] == FFEffectFilterMode::Track ? NextBasePitchBatch<Global>(s) : FBBatch<float>(0.0f);
            auto coarsePlain = topo.NormalizedToLinearFast(FFEffectParam::CombPitchCoarsePlus, combPitchCoarsePlusNormModulated[i].Load(s));
            auto trackAgainst = _filterMode[i] == FFEffectFilterMode::Track ? basePitch : coarsePlain;
            coarsePlain += (trackAgainst - (trkk + 60.0f)) * ktrk;
            coarsePlain = xsimd::clip(coarsePlain, FBBatch<float>(-FFModCoarseSemis), FBBatch<float>(FFModCoarseSemis));
            combPitchCoarsePlusPlain[i].Store(s, coarsePlain);
            realFreqPlain = xsimd::clip(FBPitchToFreq(basePitch + coarsePlain), FBBatch<float>(FFMinCombFilterFreq), FBBatch<float>(FFMaxCombFilterFreq));
          }

          if (_graph)
          {
            realFreqPlain *= _graphCombFilterFreqMultiplier;
            FBBatch<float> clampMin = 1.01f * _graphCombFilterFreqMultiplier * FFMinCombFilterFreq;
            FBBatch<float> clampMax = 0.99f * _graphCombFilterFreqMultiplier * FFMaxCombFilterFreq;
            realFreqPlain = xsimd::clip(realFreqPlain, clampMin, clampMax);
          }

          combRealFreqPlusPlain[i].Store(s, realFreqPlain);
        }
      }
      else if (FFEffectKindIsShaper(_kind[i]))
      {
        distAmtPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::DistAmt,
          FFSelectDualProcAccParamNormalized<Global>(distAmtNorm[i], voice), s));
        distMixPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::DistMix,
          FFSelectDualProcAccParamNormalized<Global>(distMixNorm[i], voice), s));
        distBiasPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::DistBias,
          FFSelectDualProcAccParamNormalized<Global>(distBiasNorm[i], voice), s));
        distDriveNormModulated[i].Store(s, FFSelectDualProcAccParamNormalized<Global>(distDriveNormIn[i], voice).CV().Load(s));

        if (!_graph)
        {
          distDriveNormModulated[i].Store(s,
            FFModulate(FFModulationOpType::BPStack, lfoOutput[i].outputAll.Load(s),
              FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
              FFSelectDualProcAccParamNormalized<Global>(distDriveNormIn[i], voice).CV().Load(s)));

          if constexpr (!Global)
          {
            distDriveNormModulated[i].Store(s,
              FFModulate(FFModulationOpType::UPStack, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                distDriveNormModulated[i].Load(s)));

            int uniTargetParamBase = (int)FFGlobalUniTarget::VFXParamA;
            FFGlobalUniTarget uniTargetParam = (FFGlobalUniTarget)(uniTargetParamBase + i);
            procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, distDriveNormModulated[i]);
          }
        }
        distDrivePlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::DistDrive, distDriveNormModulated[i].Load(s)));
      }
      else
        FB_ASSERT(_kind[i] == FFEffectKind::Off || _kind[i] == FFEffectKind::Compressor);
    }
  }

  if (_oversampleTimes != 1)
  {
    _basePitch.UpsampleStretch<FFEffectOversampleTimes>();
    trackingKeyPlain.UpsampleStretch<FFEffectOversampleTimes>();
    for (int i = 0; i < FFEffectBlockCount; i++)
    {
      if (FFEffectKindIsSVF(_kind[i]))
      {
        stVarResPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        stVarGainPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        stVarKeyTrkPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        stVarRealFreqPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
      }
      else if (FFEffectKindIsComb(_kind[i]))
      {
        combKeyTrkPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        if (_kind[i] == FFEffectKind::Comb ||
          _kind[i] == FFEffectKind::CombMin)
        {
          combResMinPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
          combRealFreqMinPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        }
        if (_kind[i] == FFEffectKind::Comb ||
          _kind[i] == FFEffectKind::CombPlus)
        {
          combResPlusPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
          combRealFreqPlusPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        }
      }
      else if (FFEffectKindIsShaper(_kind[i]))
      {
        distAmtPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        distMixPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        distBiasPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        distDrivePlain[i].UpsampleStretch<FFEffectOversampleTimes>();
      }
      else
        FB_ASSERT(_kind[i] == FFEffectKind::Off || _kind[i] == FFEffectKind::Compressor);
    }
  }

  AudioBlock<float> oversampledBlock = {};
  FBSArray2<float, FFEffectFixedBlockOversamples, 2> oversampled;
  if (_oversampleTimes == 1)
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        oversampled[c].Store(s, input[c].Load(s));
  else
  {
    float const* audioIn[2] = {};
    audioIn[0] = input[0].Ptr(0);
    audioIn[1] = input[1].Ptr(0);
    AudioBlock<float const> inputBlock(audioIn, 2, 0, FBFixedBlockSamples);
    oversampledBlock = _oversampler.processSamplesUp(inputBlock);
    for (int c = 0; c < 2; c++)
      for (int s = 0; s < FFEffectFixedBlockOversamples; s++)
        oversampled[c].Set(s, oversampledBlock.getSample(c, s));
  }

  FFEffectExchangeState* exchangeDSP = nullptr;
  float oversampledRate = _oversampleTimes * sampleRate;
  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if(exchangeToGUI != nullptr)
    exchangeDSP = FFSelectDualState<Global>(
      [exchangeToGUI, &state]() { return &exchangeToGUI->global.gEffect[state.moduleSlot]; },
      [exchangeToGUI, &state, voice]() { return &exchangeToGUI->voice[voice].vEffect[state.moduleSlot]; });
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    switch (_kind[i])
    {
    case FFEffectKind::Off:
      break;
    case FFEffectKind::Fold:
      ProcessFold(i, oversampled, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::Clip:
      ProcessClip(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::Skew:
      ProcessSkew(i, oversampled, distAmtPlain, distMixPlain, distBiasPlain, distDrivePlain);
      break;
    case FFEffectKind::Comb:
      ProcessComb<true, true>(i, oversampledRate, oversampled, combResMinPlain, combResPlusPlain, combRealFreqMinPlain, combRealFreqPlusPlain);
      break;
    case FFEffectKind::CombPlus:
      ProcessComb<true, false>(i, oversampledRate, oversampled, combResMinPlain, combResPlusPlain, combRealFreqMinPlain, combRealFreqPlusPlain);
      break;
    case FFEffectKind::CombMin:
      ProcessComb<false, true>(i, oversampledRate, oversampled, combResMinPlain, combResPlusPlain, combRealFreqMinPlain, combRealFreqPlusPlain);
      break;
    case FFEffectKind::Compressor:
      ProcessCompress(i, Global, state, oversampled);
      break;
    default:
      FB_ASSERT(FFEffectKindIsSVF(_kind[i]));
      ProcessStVar(i, oversampledRate, oversampled, stVarResPlain, stVarRealFreqPlain, stVarGainPlain);
      break;
    }
    if(exchangeDSP != nullptr)
      exchangeDSP->outputs[i] = std::max(std::abs(oversampled[0].Get(0)), std::abs(oversampled[1].Get(0)));
  }

  if(_oversampleTimes == 1)
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

  if (exchangeToGUI == nullptr)
  {
    _graphSamplesProcessed += FBFixedBlockSamples;
    return std::clamp(_graphSampleCount - _graphSamplesProcessed, 0, FBFixedBlockSamples);
  }

  exchangeDSP->boolIsActive = 1;
  exchangeDSP->basePitch = _basePitch.Get(FBFixedBlockSamples - 1);
  exchangeDSP->lengthSamples = FBTimeToSamples(FFEffectPlotLengthSeconds, sampleRate);
  exchangeDSP->output = std::max(std::abs(output[0].Get(0)), std::abs(output[1].Get(0)));

  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    exchangeDSP->shaperDrives[i] = distDrivePlain[i].First();
    exchangeDSP->stVarFreqs[i] = stVarRealFreqPlain[i].First();
    exchangeDSP->combMinFreqs[i] = combRealFreqMinPlain[i].First();
    exchangeDSP->combPlusFreqs[i] = combRealFreqPlusPlain[i].First();
#if false // TODO
    exchangeDSP->compEnvs[i] = _compEnvs[i];
#endif
  }

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gEffect[state.moduleSlot]; },
    [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vEffect[state.moduleSlot]; });
  FFSelectDualExchangeState<Global>(exchangeParams.acc.trackingKey[0], voice) = trackingKeyNorm.Last();
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    // Need to translate filter freqs/pitches back to normalized because keytracking is applied on plain, not normalized.
    if (_on && FFEffectKindIsSVF(_kind[i]) && _filterMode[i] == FFEffectFilterMode::Freq)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarFreqFreq[i], voice) =
        topo.params[(int)FFEffectParam::StVarFreqFreq].Log2().PlainToNormalizedFast(stVarFreqFreqPlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && FFEffectKindIsSVF(_kind[i]) && _filterMode[i] != FFEffectFilterMode::Freq)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarPitchCoarse[i], voice) =
      topo.params[(int)FFEffectParam::StVarPitchCoarse].Linear().PlainToNormalizedFast(stVarPitchCoarsePlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && (_kind[i] == FFEffectKind::Comb || _kind[i] == FFEffectKind::CombMin) && _filterMode[i] == FFEffectFilterMode::Freq)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.combFreqFreqMin[i], voice) =
        topo.params[(int)FFEffectParam::CombFreqFreqMin].Log2().PlainToNormalizedFast(combFreqFreqMinPlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && (_kind[i] == FFEffectKind::Comb || _kind[i] == FFEffectKind::CombMin) && _filterMode[i] != FFEffectFilterMode::Freq)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.combPitchCoarseMin[i], voice) =
        topo.params[(int)FFEffectParam::CombPitchCoarseMin].Linear().PlainToNormalizedFast(combPitchCoarseMinPlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && (_kind[i] == FFEffectKind::Comb || _kind[i] == FFEffectKind::CombPlus) && _filterMode[i] == FFEffectFilterMode::Freq)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.combFreqFreqPlus[i], voice) =
        topo.params[(int)FFEffectParam::CombFreqFreqPlus].Log2().PlainToNormalizedFast(combFreqFreqPlusPlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && (_kind[i] == FFEffectKind::Comb || _kind[i] == FFEffectKind::CombPlus) && _filterMode[i] != FFEffectFilterMode::Freq)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.combPitchCoarsePlus[i], voice) =
        topo.params[(int)FFEffectParam::CombPitchCoarsePlus].Linear().PlainToNormalizedFast(combPitchCoarsePlusPlain[i].Get(FBFixedBlockSamples - 1));

    FFSelectDualExchangeState<Global>(exchangeParams.acc.distDrive[i], voice) = distDriveNormModulated[i].Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.envAmt[i], voice) = FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.lfoAmt[i], voice) = FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.distAmt[i], voice) = FFSelectDualProcAccParamNormalized<Global>(distAmtNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.distMix[i], voice) = FFSelectDualProcAccParamNormalized<Global>(distMixNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.distBias[i], voice) = FFSelectDualProcAccParamNormalized<Global>(distBiasNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarRes[i], voice) = FFSelectDualProcAccParamNormalized<Global>(stVarResNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarGain[i], voice) = FFSelectDualProcAccParamNormalized<Global>(stVarGainNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarKeyTrk[i], voice) = FFSelectDualProcAccParamNormalized<Global>(stVarKeyTrkNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combKeyTrk[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combKeyTrkNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combResMin[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combResMinNorm[i], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.combResPlus[i], voice) = FFSelectDualProcAccParamNormalized<Global>(combResPlusNorm[i], voice).Last();
  }
  return FBFixedBlockSamples;
}

template <bool PlusOn, bool MinOn>
void
FFEffectProcessor::ProcessComb(
  int block, float oversampledRate,
  FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combResMinPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combResPlusPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combFreqMinPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combFreqPlusPlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s++)
  {
    auto resMin = combResMinPlain[block].Get(s);
    auto resPlus = combResPlusPlain[block].Get(s);
    auto freqMin = combFreqMinPlain[block].Get(s);
    auto freqPlus = combFreqPlusPlain[block].Get(s);   

    _combFilters[block].SetMin<MinOn>(oversampledRate, freqMin, resMin);
    _combFilters[block].SetPlus<PlusOn>(oversampledRate, freqPlus, resPlus);
    for (int c = 0; c < 2; c++)
      oversampled[c].Set(s, _combFilters[block].Next<PlusOn, MinOn>(c, oversampled[c].Get(s)));
  }
}

void
FFEffectProcessor::ProcessStVar(
  int block, float oversampledRate,
  FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarResPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarFreqPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarGainPlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s++)
  {
    auto res = stVarResPlain[block].Get(s);
    auto freq = stVarFreqPlain[block].Get(s);
    auto gain = stVarGainPlain[block].Get(s);
    _stVarFilters[block].Set(EffectKindToSVFMode(_kind[block]), oversampledRate, freq, res, gain);
    for (int c = 0; c < 2; c++)
      oversampled[c].Set(s, static_cast<float>(_stVarFilters[block].Next(c, oversampled[c].Get(s))));
  }
}

void
FFEffectProcessor::ProcessSkew(
  int block,
  FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  FBBatch<float> exceedBatch;
  FBBoolBatch<float> compBatch;
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
  {
    auto mix = distMixPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    {
      auto inBatch = oversampled[c].Load(s);
      auto shapedBatch = (inBatch + bias) * drive;
      auto signBatch = xsimd::sign(shapedBatch);
      auto expoBatch = xsimd::log(0.01f + distAmtPlain[block].Load(s) * 0.98f) * FBInvLogHalf;
      switch (_skewMode[block])
      {
      case FFEffectSkewMode::Bi:
        shapedBatch = signBatch * xsimd::pow(xsimd::abs(shapedBatch), expoBatch);
        break;
      case FFEffectSkewMode::Uni:
        compBatch = xsimd::lt(shapedBatch, FBBatch<float>(-1.0f));
        compBatch = xsimd::bitwise_or(compBatch, xsimd::gt(shapedBatch, FBBatch<float>(1.0f)));
        exceedBatch = FBToBipolar(xsimd::pow(FBToUnipolar(shapedBatch), expoBatch));
        shapedBatch = xsimd::select(compBatch, shapedBatch, exceedBatch);
        break;
      default:
        FB_ASSERT(false);
        break;
      }
      auto mixedBatch = (1.0f - mix) * inBatch + mix * shapedBatch;
      oversampled[c].Store(s, mixedBatch);
    }
  }
}

void
FFEffectProcessor::ProcessClip(
  int block,
  FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  FBBatch<float> tsqBatch;
  FBBatch<float> signBatch;
  FBBatch<float> exceedBatch1;
  FBBatch<float> exceedBatch2;
  FBBoolBatch<float> compBatch1;
  FBBoolBatch<float> compBatch2;

  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
  {
    auto mix = distMixPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    {
      auto inBatch = oversampled[c].Load(s);
      auto shapedBatch = (inBatch + bias) * drive;
      switch (_clipMode[block])
      {
      case FFEffectClipMode::TanH:
        shapedBatch = xsimd::tanh(shapedBatch);
        break;
      case FFEffectClipMode::Hard:
        shapedBatch = xsimd::clip(shapedBatch, FBBatch<float>(-1.0f), FBBatch<float>(1.0f));
        break;
      case FFEffectClipMode::Inv:
        shapedBatch = xsimd::sign(shapedBatch) * (1.0f - (1.0f / (1.0f + xsimd::abs(30.0f * shapedBatch))));
        break;
      case FFEffectClipMode::Sin:
        signBatch = xsimd::sign(shapedBatch);
        exceedBatch1 = xsimd::sin((shapedBatch * 3.0f * FBPi) / 4.0f);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        shapedBatch = xsimd::select(compBatch1, signBatch, exceedBatch1);
        break;
      case FFEffectClipMode::Cube:
        signBatch = xsimd::sign(shapedBatch);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        exceedBatch1 = (9.0f * shapedBatch * 0.25f) - (27.0f * shapedBatch * shapedBatch * shapedBatch / 16.0f);
        shapedBatch = xsimd::select(compBatch1, signBatch, exceedBatch1);
        break;
      case FFEffectClipMode::TSQ:
        signBatch = xsimd::sign(shapedBatch);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        compBatch2 = xsimd::lt(FBBatch<float>(-1.0f / 3.0f), shapedBatch);
        compBatch2 = xsimd::bitwise_and(compBatch2, xsimd::lt(shapedBatch, FBBatch<float>(1.0f / 3.0f)));
        exceedBatch1 = 2.0f * shapedBatch;
        exceedBatch2 = 2.0f - xsimd::abs(3.0f * shapedBatch);
        tsqBatch = signBatch * (3.0f - exceedBatch2 * exceedBatch2) / 3.0f;
        shapedBatch = xsimd::select(compBatch1, signBatch, xsimd::select(compBatch2, exceedBatch1, tsqBatch));
        break;
      case FFEffectClipMode::Exp:
        signBatch = xsimd::sign(shapedBatch);
        compBatch1 = xsimd::gt(xsimd::abs(shapedBatch), FBBatch<float>(2.0f / 3.0f));
        exceedBatch1 = signBatch * (1.0f - xsimd::pow(xsimd::abs(1.5f * shapedBatch - signBatch), 0.1f + distAmtPlain[block].Load(s) * 9.9f));
        shapedBatch = xsimd::select(compBatch1, signBatch, exceedBatch1);
        break;
      default:
        FB_ASSERT(false);
        break;
      }
      auto mixedBatch = (1.0f - mix) * inBatch + mix * shapedBatch;
      oversampled[c].Store(s, mixedBatch);
    }
  }
}

void
FFEffectProcessor::ProcessFold(
  int block,
  FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain)
{
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
  {
    auto mix = distMixPlain[block].Load(s);
    auto bias = distBiasPlain[block].Load(s);
    auto drive = distDrivePlain[block].Load(s);
    for (int c = 0; c < 2; c++)
    {
      auto inBatch = oversampled[c].Load(s);
      auto shapedBatch = (inBatch + bias) * drive;
      switch (_foldMode[block])
      {
      case FFEffectFoldModeFold: shapedBatch = FoldBack(shapedBatch); break;
      default: shapedBatch = FFCalcTrig(_foldMode[block], shapedBatch * FBPi); break;
      }
      auto mixedBatch = (1.0f - mix) * inBatch + mix * shapedBatch;
      oversampled[c].Store(s, mixedBatch);
    }
  }
}

// basic algo https://github.com/jonathonracz/GoatMix/tree/master/GoatMix/Source/External/SimpleComp
// soft knee https://github.com/tu-studio/IEMPluginSuite/blob/master/resources/Compressor.h
void 
FFEffectProcessor::ProcessCompress(
  int block, bool global, FBModuleProcState const& state,
  FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled)
{
  (void)state;
  (void)global;
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;

  for (int s = 0; s < totalSamples; s++)
  {
    float measure = std::max(std::abs(oversampled[0].Get(s)), oversampled[1].Get(s));
    if (!_graph && _compMode[block] == FFEffectCompMode::RMS)
    {
      _compRMSTotal[block] -= _compRMSWindows[block][_compRMSWindowsPos[block]];
      _compRMSTotal[block] += measure * measure;
      _compRMSWindows[block][_compRMSWindowsPos[block]] = measure * measure;
      _compRMSWindowsPos[block]++;
      _compRMSWindowsPos[block] %= (int)_compRMSWindows[block].size();
      measure = std::sqrt(_compRMSTotal[block] / (float)_compRMSWindows[block].size());
    }

    float measureDb = 20.0f * std::log10(measure);
    float overDb = measureDb - _compThresholdDb[block];
    float ratio = 1.0f / (1.0f - _compRatio[block] * 0.5f);
    float slope = ratio - 1.0f;
    float kneeHalf = _compKneeDb[block] * 0.5f;
    if (overDb <= -kneeHalf)
      overDb = 0.0f;
    else if (overDb > -kneeHalf && overDb <= kneeHalf)
      overDb = 0.5f * slope * juce::square(overDb + kneeHalf) / _compKneeDb[block];
    else
      overDb = slope * overDb;
    if (overDb > _compEnvStateDb[block])
      _compEnvStateDb[block] = overDb + _compEnvCoeffAttack[block] * (_compEnvStateDb[block] - overDb);
    else
      _compEnvStateDb[block] = overDb + _compEnvCoeffRelease[block] * (_compEnvStateDb[block] - overDb);
    overDb = _compEnvStateDb[block];    
    float gainReductionDb = overDb * (ratio - 1.0f);	
    float gainReduction = std::pow(10.0f, -gainReductionDb / 20.0f);
    oversampled[0].Set(s, oversampled[0].Get(s) * gainReduction);
    oversampled[1].Set(s, oversampled[1].Get(s) * gainReduction);
  }

#if false // TODO
  oversampled.SanityCheck();

  bool haveSide = true;
  auto procState = state.ProcAs<FFProcState>();
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  FBSArray2<float, FBFixedBlockSamples, 2> sideDetector = {};
  FBSArray2<float, FFEffectFixedBlockOversamples, 2> detector = {};

  if (_graph || (
    global && _gCompSide[block] == FFGEffectCompSide::Off ||
    !global && _vCompSide[block] == FFVEffectCompSide::Off))
  {
    haveSide = false;
    for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
      for (int c = 0; c < 2; c++)
        detector[c].Store(s, oversampled[c].Load(s));
  }
  else if (global)
  {
    switch (_gCompSide[block])
    {
    case FFGEffectCompSide::AudioIn:
      state.input->mainAudio->CopyTo(sideDetector);
      break;
    case FFGEffectCompSide::Sidechain:
      state.input->sidechainAudio->CopyTo(sideDetector);
      break;
    case FFGEffectCompSide::VMix:
      // TODO
      break;
    case FFGEffectCompSide::FX1:
      procState->dsp.global.gEffect[0].output.CopyTo(sideDetector);
      break;
    case FFGEffectCompSide::FX2:
      procState->dsp.global.gEffect[1].output.CopyTo(sideDetector);
      break;
    case FFGEffectCompSide::FX3:
      procState->dsp.global.gEffect[2].output.CopyTo(sideDetector);
      break;
    default:
      FB_ASSERT(false);
      break;
    }
  }
  else
  {
    switch (_vCompSide[block])
    {
    case FFVEffectCompSide::AudioIn:
      state.input->mainAudio->CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::Sidechain:
      state.input->sidechainAudio->CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::OscMix:
      // TODO
      break;
    case FFVEffectCompSide::Osc1:
      procState->dsp.voice[state.voice->slot].osci[0].output.CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::Osc2:
      procState->dsp.voice[state.voice->slot].osci[1].output.CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::Osc3:
      procState->dsp.voice[state.voice->slot].osci[2].output.CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::Osc4:
      procState->dsp.voice[state.voice->slot].osci[3].output.CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::FX1:
      procState->dsp.voice[state.voice->slot].vEffect[0].output.CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::FX2:
      procState->dsp.voice[state.voice->slot].vEffect[1].output.CopyTo(sideDetector);
      break;
    case FFVEffectCompSide::FX3:
      procState->dsp.voice[state.voice->slot].vEffect[2].output.CopyTo(sideDetector);
      break;
    default:
      FB_ASSERT(false);
      break;
    }
  }

  if (haveSide)
  {
    for (int i = 0; i < FBFixedBlockSamples; i++)
      for (int c = 0; c < 2; c++)
        detector[c].Set(i, sideDetector[c].Get(i));
    if (_oversampleTimes != 1)
    {
      detector[0].UpsampleStretch<FFEffectOversampleTimes>();
      detector[1].UpsampleStretch<FFEffectOversampleTimes>();
    }
  }

  for (int s = 0; s < totalSamples; s++)
  {
    float ratio = compRatioPlain[block].Get(s);
    float kneeDb = compKneePlain[block].Get(s);
    float threshold = compThresholdPlain[block].Get(s);
    float measure = std::max(std::abs(detector[0].Get(s)), detector[1].Get(s));
    if (!_graph && _compMode[block] == FFEffectCompMode::RMS)
    {
      _compRMSTotal[block] -= _compRMSWindows[block][_compRMSWindowsPos[block]];
      _compRMSTotal[block] += measure * measure;
      _compRMSWindows[block][_compRMSWindowsPos[block]] = measure * measure;
      _compRMSWindowsPos[block]++;
      _compRMSWindowsPos[block] %= (int)_compRMSWindows[block].size();
      measure = std::sqrt(_compRMSTotal[block] / (float)_compRMSWindows[block].size());
    }

    if (measure > threshold)
    {
      if (_compStage[block] == FFEffectCompStage::Off)
      {
        _compEnvStart[block] = 0.0f;
        _compStage[block] = FFEffectCompStage::Attack;
        _compAttackPositionSamplesOversampled[block] = 0;
        _compReleasePositionSamplesOversampled[block] = 0;
      }
      else if (_compStage[block] == FFEffectCompStage::Attack)
      {
        _compStage[block] = FFEffectCompStage::Attack;
        _compReleasePositionSamplesOversampled[block] = 0;
      }
      else if (_compStage[block] == FFEffectCompStage::Release)
      {
        _compStage[block] = FFEffectCompStage::Attack;
        float releasePos = _compReleasePositionSamplesOversampled[block] / (float)_compReleaseSamplesOversampled[block];
        _compEnvStart[block] = 1.0f - releasePos;
        _compAttackPositionSamplesOversampled[block] = 0;
      }
    }

    float measureDb = 20.0f * std::log10(measure);
    float thresholdDb = 20.0f * std::log10(threshold);
    float thresholdStartDb = thresholdDb - kneeDb * 0.5f;
    if (measureDb >= thresholdStartDb)
    {
      float yDb = 0.0f;
      float thresholdEndDb = thresholdDb + kneeDb * 0.5f;
      if (measureDb <= thresholdEndDb)
      {
        float z = (measureDb - thresholdDb + kneeDb * 0.5f);
        yDb = measureDb - ratio * z * z / (2.0f * kneeDb);
      }
      else
      {
        yDb = thresholdDb + (measureDb - thresholdDb) * (1.0f - ratio);
      }
      float gain = std::pow(10.0f, yDb / 20.0f) / measure;
      if (!_graph)
        gain = (1.0f - _compEnvs[block]) + _compEnvs[block] * gain;
      for (int c = 0; c < 2; c++)
      {
        FB_ASSERT(!std::isnan(gain));
        oversampled[c].Set(s, oversampled[c].Get(s) * gain);
      }
    }    

    if (_compStage[block] == FFEffectCompStage::Attack)
    {
      if (_compAttackPositionSamplesOversampled[block] < _compAttackSamplesOversampled[block])
        _compAttackPositionSamplesOversampled[block]++;
      if (_compAttackPositionSamplesOversampled[block] >= _compAttackSamplesOversampled[block])
      {
        _compStage[block] = FFEffectCompStage::Release;
        _compAttackPositionSamplesOversampled[block] = 0;
        _compReleasePositionSamplesOversampled[block] = 0;
      }
    }
    if (_compStage[block] == FFEffectCompStage::Release)
    {
      if (_compReleasePositionSamplesOversampled[block] < _compReleaseSamplesOversampled[block])
        _compReleasePositionSamplesOversampled[block]++;
      if (_compReleasePositionSamplesOversampled[block] >= _compReleaseSamplesOversampled[block])
      {
        _compStage[block] = FFEffectCompStage::Off;
        _compAttackPositionSamplesOversampled[block] = 0;
        _compReleasePositionSamplesOversampled[block] = 0;
      }
    }
    if (_compStage[block] == FFEffectCompStage::Off)
      _compEnvs[block] = 0.0f;
    else if (_compStage[block] == FFEffectCompStage::Attack)
      _compEnvs[block] = _compEnvStart[block] + (1.0f - _compEnvStart[block]) * _compAttackPositionSamplesOversampled[block] / (float)_compAttackSamplesOversampled[block];
    else if (_compStage[block] == FFEffectCompStage::Release)
      _compEnvs[block] = 1.0f - _compReleasePositionSamplesOversampled[block] / (float)_compReleaseSamplesOversampled[block];
    else
      FB_ASSERT(false);
  }

  oversampled.SanityCheck();
#endif
}

template void FFEffectProcessor::BeginVoiceOrBlock<true>(FBModuleProcState&, bool, bool, int, int);
template void FFEffectProcessor::BeginVoiceOrBlock<false>(FBModuleProcState&, bool, bool, int, int);
template int FFEffectProcessor::Process<true>(FBModuleProcState&, FFEffectExchangeState const*, bool);
template int FFEffectProcessor::Process<false>(FBModuleProcState&, FFEffectExchangeState const*, bool);
template void FFEffectProcessor::AllocOnDemandBuffers<true>(FBRuntimeTopo const*, FBProcStateContainer*, int, bool, float);
template void FFEffectProcessor::AllocOnDemandBuffers<false>(FBRuntimeTopo const*, FBProcStateContainer*, int, bool, float);