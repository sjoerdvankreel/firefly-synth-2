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
float 
FFEffectProcessor::NextBasePitchScalar(int sample)
{
  if constexpr (Global)
    return _basePitchSmoother.NextScalar(_basePitch.Get(sample));
  else
    return _basePitch.Get(sample);
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
    _combFilters[i].ReleaseBuffers(state->MemoryPool());
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
    if (graph || (
      kind == FFEffectKind::CombFreq || kind == FFEffectKind::CombPitch || 
      kind == FFEffectKind::CombMinFreq || kind == FFEffectKind::CombMinPitch ||
      kind == FFEffectKind::CombPlusFreq || kind == FFEffectKind::CombPlusPitch))
      _combFilters[i].AllocBuffers(state->MemoryPool(), sampleRate * FFEffectOversampleTimes, FFMinCombFilterFreq * graphFilterFreqMultiplier); 
  }
}

template <bool Global>
void
FFEffectProcessor::BeginVoiceOrBlock(
  FBModuleProcState& state, bool graph, int graphIndex, int graphSampleCount)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gEffect[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vEffect[state.moduleSlot]; });
  auto const& topo = state.topo->static_->modules[(int)(Global? FFModuleType::GEffect: FFModuleType::VEffect)];
  
  auto const& kindNorm = params.block.kind;
  auto const& clipModeNorm = params.block.clipMode;
  auto const& foldModeNorm = params.block.foldMode;
  auto const& skewModeNorm = params.block.skewMode;
  auto const& stVarModeNorm = params.block.stVarMode;
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
    bool blockActive = !graph || graphIndex == i || graphIndex == FFEffectBlockCount;
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
    _stVarMode[i] = topo.NormalizedToListFast<FFStateVariableFilterMode>(
      FFEffectParam::StVarMode, 
      FFSelectDualProcBlockParamNormalized<Global>(stVarModeNorm[i], voice));

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
  auto const& combFreqFreqPlusNormIn = procParams.acc.combFreqFreqPlus;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> distDriveNormModulated = {};
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarFreqFreqNormModulated = {};
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarPitchCoarseNormModulated = {};
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combFreqFreqMinNormModulated = {};
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> combFreqFreqPlusNormModulated = {};

  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarFreqFreqPlain;
  FBSArray2<float, FBFixedBlockSamples, FFEffectBlockCount> stVarPitchCoarsePlain;
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
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> combFreqFreqMinPlain;
  FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> combFreqFreqPlusPlain;

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
      for (int s = 0; s < FBFixedBlockSamples; s++)
        _basePitch.Set(s, state.input->noteMatrixRaw->entries[(int)FBNoteMatrixEntry::LastKeyUntuned].Get(s) * 127.0f);
      auto masterPitchBendTarget = procState->dsp.global.master.bendTarget;
      if (masterPitchBendTarget == FFMasterPitchBendTarget::Global)
        for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
          _basePitch.Add(s, procState->dsp.global.master.bendAmountInSemis.Load(s));
    }
    else
    {
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        _basePitch.Store(s, procState->dsp.voice[voice].voiceModule.basePitchSemis.Load(s));
    }
  }

  if (_oversampleTimes != 1)
    _basePitch.UpsampleStretch<FFEffectOversampleTimes>();
  
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    trackingKeyPlain.Store(s, topo.NormalizedToLinearFast(FFEffectParam::TrackingKey, trackingKeyNorm, s));
    for (int i = 0; i < FFEffectBlockCount; i++)
    {
      combFreqFreqMinNormModulated[i].Store(s, FFSelectDualProcAccParamNormalized<Global>(combFreqFreqMinNormIn[i], voice).CV().Load(s));
      combFreqFreqPlusNormModulated[i].Store(s, FFSelectDualProcAccParamNormalized<Global>(combFreqFreqPlusNormIn[i], voice).CV().Load(s));
      if (_kind[i] == FFEffectKind::StVarFreq || _kind[i] == FFEffectKind::StVarPitch)
      {
        stVarResPlain[i].Store(s, topo.NormalizedToIdentityFast(FFEffectParam::StVarRes,
          FFSelectDualProcAccParamNormalized<Global>(stVarResNorm[i], voice), s));
        stVarGainPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::StVarGain,
          FFSelectDualProcAccParamNormalized<Global>(stVarGainNorm[i], voice), s));
        stVarKeyTrkPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::StVarKeyTrak,
          FFSelectDualProcAccParamNormalized<Global>(stVarKeyTrkNorm[i], voice), s));

        if(_kind[i] == FFEffectKind::StVarFreq)
          stVarFreqFreqNormModulated[i].Store(s, FFSelectDualProcAccParamNormalized<Global>(stVarFreqFreqNormIn[i], voice).CV().Load(s));
        else
          stVarPitchCoarseNormModulated[i].Store(s, FFSelectDualProcAccParamNormalized<Global>(stVarPitchCoarseNormIn[i], voice).CV().Load(s));

        if (!_graph)
        {
          if (_kind[i] == FFEffectKind::StVarFreq)
            stVarFreqFreqNormModulated[i].Store(s,
              FFModulate(FFModulationOpType::UPMul, lfoOutput[i].outputAll.Load(s),
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
            if (_kind[i] == FFEffectKind::StVarFreq)
            {
              stVarFreqFreqNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::UPMul, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
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
        if (_kind[i] == FFEffectKind::StVarFreq)
        {
          auto freqFreqPlain = topo.NormalizedToLog2Fast(FFEffectParam::StVarFreqFreq, stVarFreqFreqNormModulated[i].Load(s));
          freqFreqPlain = FFMultiplyClamp(freqFreqPlain, FFKeyboardTrackingMultiplier(NextBasePitchBatch<Global>(s), trkk, ktrk),
            FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
          stVarFreqFreqPlain[i].Store(s, freqFreqPlain);
          realFreqPlain = freqFreqPlain;
        }
        else
        {
          auto basePitch = NextBasePitchBatch<Global>(s);
          auto coarsePlain = topo.NormalizedToLinearFast(FFEffectParam::StVarPitchCoarse, stVarPitchCoarseNormModulated[i].Load(s));
          coarsePlain += (basePitch - (trkk + 60.0f)) * ktrk;
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
      else if (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombPitch || 
        _kind[i] == FFEffectKind::CombPlusFreq || _kind[i] == FFEffectKind::CombPlusPitch || 
        _kind[i] == FFEffectKind::CombMinFreq || _kind[i] == FFEffectKind::CombMinPitch)
      {
        combKeyTrkPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombKeyTrk,
          FFSelectDualProcAccParamNormalized<Global>(combKeyTrkNorm[i], voice), s));
        if (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombPitch || 
          _kind[i] == FFEffectKind::CombMinFreq || _kind[i] == FFEffectKind::CombMinPitch)
        {
          combResMinPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombResMin,
            FFSelectDualProcAccParamNormalized<Global>(combResMinNorm[i], voice), s));

          if (!_graph)
          {
            combFreqFreqMinNormModulated[i].Store(s,
              FFModulate(FFModulationOpType::UPMul, lfoOutput[i].outputAll.Load(s),
                FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                FFSelectDualProcAccParamNormalized<Global>(combFreqFreqMinNormIn[i], voice).CV().Load(s)));

            if constexpr (!Global)
            {
              combFreqFreqMinNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::UPMul, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                  combFreqFreqMinNormModulated[i].Load(s)));

              int uniTargetParamBase = (int)FFGlobalUniTarget::VFXParamA;
              FFGlobalUniTarget uniTargetParam = (FFGlobalUniTarget)(uniTargetParamBase + i);
              procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, combFreqFreqMinNormModulated[i]);
            }
          }        

          auto freqPlain = topo.NormalizedToLog2Fast(FFEffectParam::CombFreqFreqMin, combFreqFreqMinNormModulated[i].Load(s));
          if (_graph)
          {
            float clampMin = 1.01f * _graphCombFilterFreqMultiplier * FFMinCombFilterFreq;
            float clampMax = 0.99f * _graphCombFilterFreqMultiplier * FFMaxCombFilterFreq;
            freqPlain = FFMultiplyClamp(freqPlain, _graphCombFilterFreqMultiplier, clampMin, clampMax);
          }
          else
          {
            auto trkk = trackingKeyPlain.Load(s);
            auto ktrk = combKeyTrkPlain[i].Load(s);
            auto freqMul = FFKeyboardTrackingMultiplier(NextBasePitchBatch<Global>(s), trkk, ktrk);
            freqPlain = FFMultiplyClamp(freqPlain, freqMul, FFMinCombFilterFreq, FFMaxCombFilterFreq);
          }
          combFreqFreqMinPlain[i].Store(s, freqPlain);
        }
        if (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombPitch ||
          _kind[i] == FFEffectKind::CombPlusFreq || _kind[i] == FFEffectKind::CombPlusPitch)
        {
          combResPlusPlain[i].Store(s, topo.NormalizedToLinearFast(FFEffectParam::CombResPlus,
            FFSelectDualProcAccParamNormalized<Global>(combResPlusNorm[i], voice), s));

          if (!_graph)
          {
            combFreqFreqPlusNormModulated[i].Store(s,
              FFModulate(FFModulationOpType::UPMul, lfoOutput[i].outputAll.Load(s),
                FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
                FFSelectDualProcAccParamNormalized<Global>(combFreqFreqPlusNormIn[i], voice).CV().Load(s)));

            if constexpr (!Global)
            {
              combFreqFreqPlusNormModulated[i].Store(s,
                FFModulate(FFModulationOpType::UPMul, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
                  FFSelectDualProcAccParamNormalized<Global>(envAmtNorm[i], voice).CV().Load(s),
                  combFreqFreqPlusNormModulated[i].Load(s)));

              int uniTargetParamBase = (int)FFGlobalUniTarget::VFXParamA;
              FFGlobalUniTarget uniTargetParam = (FFGlobalUniTarget)(uniTargetParamBase + i);
              procState->dsp.global.globalUni.processor->ApplyToVoice(state, uniTargetParam, false, voice, -1, combFreqFreqPlusNormModulated[i]);
            }
          }

          auto freqPlain = topo.NormalizedToLog2Fast(FFEffectParam::CombFreqFreqPlus, combFreqFreqPlusNormModulated[i].Load(s));
          if (_graph)
          {
            float clampMin = 1.01f * _graphCombFilterFreqMultiplier * FFMinCombFilterFreq;
            float clampMax = 0.99f * _graphCombFilterFreqMultiplier * FFMaxCombFilterFreq;
            freqPlain = FFMultiplyClamp(freqPlain, _graphCombFilterFreqMultiplier, clampMin, clampMax);
          }
          else
          {
            auto trkk = trackingKeyPlain.Load(s);
            auto ktrk = combKeyTrkPlain[i].Load(s);
            auto freqMul = FFKeyboardTrackingMultiplier(NextBasePitchBatch<Global>(s), trkk, ktrk);
            freqPlain = FFMultiplyClamp(freqPlain, freqMul, FFMinCombFilterFreq, FFMaxCombFilterFreq);
          }
          combFreqFreqPlusPlain[i].Store(s, freqPlain);
        }
      }
      else if (_kind[i] == FFEffectKind::Clip || _kind[i] == FFEffectKind::Fold || _kind[i] == FFEffectKind::Skew)
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
            FFModulate(FFModulationOpType::UPMul, lfoOutput[i].outputAll.Load(s),
              FFSelectDualProcAccParamNormalized<Global>(lfoAmtNorm[i], voice).CV().Load(s),
              FFSelectDualProcAccParamNormalized<Global>(distDriveNormIn[i], voice).CV().Load(s)));

          if constexpr (!Global)
          {
            distDriveNormModulated[i].Store(s,
              FFModulate(FFModulationOpType::UPMul, procState->dsp.voice[voice].env[i + FFEnvSlotOffset].output.Load(s),
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
        FB_ASSERT(_kind[i] == FFEffectKind::Off);
    }
  }

  if (_oversampleTimes != 1)
  {
    trackingKeyPlain.UpsampleStretch<FFEffectOversampleTimes>();
    for (int i = 0; i < FFEffectBlockCount; i++)
    {
      if (_kind[i] == FFEffectKind::StVarFreq || _kind[i] == FFEffectKind::StVarPitch)
      {
        stVarResPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        stVarGainPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        stVarKeyTrkPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        stVarRealFreqPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
      }
      else if (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombPitch ||
        _kind[i] == FFEffectKind::CombPlusFreq || _kind[i] == FFEffectKind::CombPlusPitch ||
        _kind[i] == FFEffectKind::CombMinFreq || _kind[i] == FFEffectKind::CombMinPitch)
      {
        combKeyTrkPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        if (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombPitch || 
          _kind[i] == FFEffectKind::CombMinFreq || _kind[i] == FFEffectKind::CombMinPitch)
        {
          combResMinPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
          combFreqFreqMinPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        }
        if (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombPitch ||
          _kind[i] == FFEffectKind::CombPlusFreq || _kind[i] == FFEffectKind::CombPlusPitch)
        {
          combResPlusPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
          combFreqFreqPlusPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        }
      }
      else if (_kind[i] == FFEffectKind::Clip || _kind[i] == FFEffectKind::Fold || _kind[i] == FFEffectKind::Skew)
      {
        distAmtPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        distMixPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        distBiasPlain[i].UpsampleStretch<FFEffectOversampleTimes>();
        distDrivePlain[i].UpsampleStretch<FFEffectOversampleTimes>();
      }
      else
        FB_ASSERT(_kind[i] == FFEffectKind::Off);
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

  float oversampledRate = _oversampleTimes * sampleRate;
  for(int i = 0; i < FFEffectBlockCount; i++)
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
    case FFEffectKind::StVarFreq:
    case FFEffectKind::StVarPitch:
      ProcessStVar<Global>(i, oversampledRate, oversampled, stVarResPlain, stVarRealFreqPlain, stVarGainPlain);
      break;
    case FFEffectKind::CombFreq:
    case FFEffectKind::CombPitch:
      ProcessComb<Global, true, true>(i, oversampledRate, oversampled, combResMinPlain, combResPlusPlain, combFreqFreqMinPlain, combFreqFreqPlusPlain);
      break;
    case FFEffectKind::CombPlusFreq:
    case FFEffectKind::CombPlusPitch:
      ProcessComb<Global, true, false>(i, oversampledRate, oversampled, combResMinPlain, combResPlusPlain, combFreqFreqMinPlain, combFreqFreqPlusPlain);
      break;
    case FFEffectKind::CombMinFreq:
    case FFEffectKind::CombMinPitch:
      ProcessComb<Global, false, true>(i, oversampledRate, oversampled, combResMinPlain, combResPlusPlain, combFreqFreqMinPlain, combFreqFreqPlusPlain);
      break;
    default:
      break;
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

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    _graphSamplesProcessed += FBFixedBlockSamples;
    return std::clamp(_graphSampleCount - _graphSamplesProcessed, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = *FFSelectDualState<Global>(
    [exchangeToGUI, &state]() { return &exchangeToGUI->global.gEffect[state.moduleSlot]; },
    [exchangeToGUI, &state, voice]() { return &exchangeToGUI->voice[voice].vEffect[state.moduleSlot]; });
  exchangeDSP.boolIsActive = 1;
  exchangeDSP.basePitch = _basePitch.Get(FBFixedBlockSamples - 1);
  exchangeDSP.lengthSamples = FBTimeToSamples(FFEffectPlotLengthSeconds, sampleRate);

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gEffect[state.moduleSlot]; },
    [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vEffect[state.moduleSlot]; });
  FFSelectDualExchangeState<Global>(exchangeParams.acc.trackingKey[0], voice) = trackingKeyNorm.Last();
  for (int i = 0; i < FFEffectBlockCount; i++)
  {
    // Need to translate filter freqs/pitches back to normalized because keytracking is applied on plain, not normalized.
    if (_on && _kind[i] == FFEffectKind::StVarFreq)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarFreqFreq[i], voice) =
        topo.params[(int)FFEffectParam::StVarFreqFreq].Log2().PlainToNormalizedFast(stVarFreqFreqPlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && _kind[i] == FFEffectKind::StVarPitch)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.stVarPitchCoarse[i], voice) =
      topo.params[(int)FFEffectParam::StVarPitchCoarse].Linear().PlainToNormalizedFast(stVarPitchCoarsePlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombMinFreq))
      FFSelectDualExchangeState<Global>(exchangeParams.acc.combFreqFreqMin[i], voice) =
        topo.params[(int)FFEffectParam::CombFreqFreqMin].Log2().PlainToNormalizedFast(combFreqFreqMinPlain[i].Get(FBFixedBlockSamples - 1));
    if (_on && (_kind[i] == FFEffectKind::CombFreq || _kind[i] == FFEffectKind::CombPlusFreq))
      FFSelectDualExchangeState<Global>(exchangeParams.acc.combFreqFreqPlus[i], voice) =
        topo.params[(int)FFEffectParam::CombFreqFreqPlus].Log2().PlainToNormalizedFast(combFreqFreqPlusPlain[i].Get(FBFixedBlockSamples - 1));

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

template <bool Global, bool PlusOn, bool MinOn>
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

template <bool Global>
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
    _stVarFilters[block].Set(_stVarMode[block], oversampledRate, freq, res, gain);
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
      auto expoBatch = xsimd::log(0.01f + distAmtPlain[block].Load(s) * 0.98f) * FFInvLogHalf;
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

template void FFEffectProcessor::BeginVoiceOrBlock<true>(FBModuleProcState&, bool, int, int);
template void FFEffectProcessor::BeginVoiceOrBlock<false>(FBModuleProcState&, bool, int, int);
template int FFEffectProcessor::Process<true>(FBModuleProcState&, FFEffectExchangeState const*, bool);
template int FFEffectProcessor::Process<false>(FBModuleProcState&, FFEffectExchangeState const*, bool);
template void FFEffectProcessor::AllocOnDemandBuffers<true>(FBRuntimeTopo const*, FBProcStateContainer*, int, bool, float);
template void FFEffectProcessor::AllocOnDemandBuffers<false>(FBRuntimeTopo const*, FBProcStateContainer*, int, bool, float);