#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

static inline FBSArray<float, FBFixedBlockSamples> const*
GetSourceCVBuffer(FBModuleProcState& state, FFModMatrixSource const& source, int voice)
{
  auto* procState = state.ProcAs<FFProcState>();
  int ms = source.indices.module.slot;
  int mi = source.indices.module.index;
  int os = source.indices.cvOutput.slot;
  int oi = source.indices.cvOutput.index;
  auto const& sourceCvOutput = state.topo->static_->modules[mi].cvOutputs[oi];
  if (source.isGlobal)
    return sourceCvOutput.globalAddr(ms, os, procState);
  else
    return sourceCvOutput.voiceAddr(ms, os, voice, procState);
}

template <bool Global>
void
FFModMatrixProcessor<Global>::InitBuffers(FBRuntimeTopo const* topo)
{
  // static_cast for perf
  // 2d vector beats map performance
  auto const& ffTopo = static_cast<FFStaticTopo const&>(*topo->static_);
  auto const& sources = Global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  for (int i = 0; i < sources.size(); i++)
  {
    auto const& modIndices = sources[i].indices.module;
    if (modIndices.index != -1)
    {
      _modSourceIsReady.resize(std::max((int)_modSourceIsReady.size(), modIndices.index + 1));
      _modSourceIsReady[modIndices.index].resize(std::max((int)_modSourceIsReady[modIndices.index].size(), modIndices.slot + 1));
    }
  }
}

template <bool Global>
void
FFModMatrixProcessor<Global>::BeginModulationBlock()
{
  // perf
  if (_activeSlotCount == 0)
    return;

  // On ApplyModulation, we can't do it right away.
  // For each slot, need to wait untill all slots with the same
  // target param have cleared their modsource, and then apply.
  // Otherwise, the stacked modes are stacking against something that has yet to come.
  // Scale is ok because it can never be "later" than the modsource.
  for (int i = 0; i < _modSourceIsReady.size(); i++)
    for (int j = 0; j < _modSourceIsReady[i].size(); j++)
      _modSourceIsReady[i][j] = 0;
  for (int i = 0; i < _activeSlotCount; i++)
  {
    _slotHasBeenProcessed[i] = false;
    _ownModSourceIsReadyForSlot[i] = false;
    _allModSourcesAreReadyForSlot[i] = false;
  }
}

template <bool Global>
void 
FFModMatrixProcessor<Global>::EndModulationBlock(FBModuleProcState& state)
{
  // perf
  if (_activeSlotCount == 0)
    return;

  auto* procStateContainer = state.input->procState;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  // static_cast for perf
  auto const& ffTopo = static_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& targets = Global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;
  for (int i = 0; i < _activeSlotCount; i++)
  {
    auto const& target = targets[_target[i]];
    if (target.module.index != -1)
    {
      int runtimeTargetParamIndex = state.topo->audio.ParamAtTopo(target)->runtimeParamIndex;
      if constexpr (Global)
        procStateContainer->Params()[runtimeTargetParamIndex].GlobalAcc().Global().ClearPlugModulation();
      else
        procStateContainer->Params()[runtimeTargetParamIndex].VoiceAcc().Voice()[voice].ClearPlugModulation();
    }
  }
}

template <bool Global>
void 
FFModMatrixProcessor<Global>::BeginVoiceOrBlock(
  FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gMatrix[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vMatrix[state.moduleSlot]; });
  auto const& topo = state.topo->static_->modules[(int)(Global ? FFModuleType::GMatrix : FFModuleType::VMatrix)];

  auto const& scaleNorm = params.block.scale;
  auto const& sourceNorm = params.block.source;
  auto const& targetNorm = params.block.target;
  auto const& opTypeNorm = params.block.opType;
  auto const& scaleMinNorm = params.acc.scaleMin;
  auto const& scaleMaxNorm = params.acc.scaleMax;
  auto const& targetAmtNorm = params.acc.targetAmt;
  auto const& sourceLowNorm = params.acc.sourceLow;
  auto const& sourceHighNorm = params.acc.sourceHigh;

  if constexpr (!Global)
    _onNoteWasSnapshotted.fill(false);

  float activeSlotsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.slots[0], voice);
  _activeSlotCount = topo.NormalizedToDiscreteFast(FFModMatrixParam::Slots, activeSlotsNorm);

  for (int i = 0; i < _activeSlotCount; i++)
  {
    _scale[i] = topo.NormalizedToListFast<int>(
      FFModMatrixParam::Scale,
      FFSelectDualProcBlockParamNormalized<Global>(scaleNorm[i], voice));
    _target[i] = topo.NormalizedToListFast<int>(
      FFModMatrixParam::Target,
      FFSelectDualProcBlockParamNormalized<Global>(targetNorm[i], voice));
    _source[i] = topo.NormalizedToListFast<int>(
      FFModMatrixParam::Source,
      FFSelectDualProcBlockParamNormalized<Global>(sourceNorm[i], voice));
    _opType[i] = topo.NormalizedToListFast<FFModulationOpType>(
      FFModMatrixParam::OpType,
      FFSelectDualProcBlockParamNormalized<Global>(opTypeNorm[i], voice));
  }

  // Prevent unscaled stuff from showing up as 0.
  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI != nullptr)
  {
    auto& exchangeParams = *FFSelectDualState<Global>(
      [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gMatrix[state.moduleSlot]; },
      [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vMatrix[state.moduleSlot]; });
    for (int i = 0; i < _activeSlotCount; i++)
    {
      FFSelectDualExchangeState<Global>(exchangeParams.acc.scaleMin[i], voice) =
        FFSelectDualProcAccParamNormalized<Global>(scaleMinNorm[i], voice).CV().Get(0);
      FFSelectDualExchangeState<Global>(exchangeParams.acc.scaleMax[i], voice) =
        FFSelectDualProcAccParamNormalized<Global>(scaleMaxNorm[i], voice).CV().Get(0);
      FFSelectDualExchangeState<Global>(exchangeParams.acc.targetAmt[i], voice) =
        FFSelectDualProcAccParamNormalized<Global>(targetAmtNorm[i], voice).CV().Get(0);
      FFSelectDualExchangeState<Global>(exchangeParams.acc.sourceLow[i], voice) =
        FFSelectDualProcAccParamNormalized<Global>(sourceLowNorm[i], voice).CV().Get(0);
      FFSelectDualExchangeState<Global>(exchangeParams.acc.sourceHigh[i], voice) =
        FFSelectDualProcAccParamNormalized<Global>(sourceHighNorm[i], voice).CV().Get(0);
    }
  }
}

template <bool Global>
void
FFModMatrixProcessor<Global>::ModSourceCleared(
  FBModuleProcState& state, FBTopoIndices const& currentModule)
{
  // perf
  if (_activeSlotCount == 0)
    return;
  _modSourceIsReady[currentModule.index][currentModule.slot] = 1;
  UpdateCandidateSlots(state);
}

template <bool Global>
void
FFModMatrixProcessor<Global>::AllGlobalModSourcesCleared(
  FBModuleProcState& state)
{
  FB_ASSERT(!Global);

  // perf
  if (_activeSlotCount == 0)
    return;

  auto const& ffTopo = static_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& sources = ffTopo.vMatrixSources;
  for (int i = 0; i < sources.size(); i++)
    if (sources[i].isGlobal && sources[i].indices.module.index != -1)
      _modSourceIsReady[sources[i].indices.module.index][sources[i].indices.module.slot] = 1;
  UpdateCandidateSlots(state);
}

template <bool Global>
void 
FFModMatrixProcessor<Global>::UpdateCandidateSlots(FBModuleProcState& state)
{
  // static_cast for perf
  auto const& ffTopo = static_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& sources = Global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  auto const& targets = Global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;

  // need all slots with same target ready before we begin processing because stacking modulators
  // dont you just love the bookkeeping?
  for (int i = 0; i < _activeSlotCount; i++)
  {
    _ownModSourceIsReadyForSlot[i] = _opType[i] != FFModulationOpType::Off;
    _allModSourcesAreReadyForSlot[i] = _opType[i] != FFModulationOpType::Off;
    if (_ownModSourceIsReadyForSlot[i])
    {
      auto const& thisTarget = targets[_target[i]];
      auto const& thisSource = sources[_source[i]];
      _ownModSourceIsReadyForSlot[i] &= thisTarget.module.index != -1;
      _ownModSourceIsReadyForSlot[i] &= thisSource.indices.module.index != -1;
      if (thisSource.indices.module.index != -1)
        _ownModSourceIsReadyForSlot[i] &= _modSourceIsReady[thisSource.indices.module.index][thisSource.indices.module.slot] != 0;
      _allModSourcesAreReadyForSlot[i] &= _ownModSourceIsReadyForSlot[i];
      if (_allModSourcesAreReadyForSlot[i])
        for (int j = 0; j < _activeSlotCount; j++)
          if (_opType[j] != FFModulationOpType::Off)
          {
            auto const& thatSource = sources[_source[j]];
            auto const& thatTarget = targets[_target[j]];
            if (thatSource.indices.module.index != -1 && thatTarget.module == thisTarget.module)
              _allModSourcesAreReadyForSlot[i] &= _modSourceIsReady[thatSource.indices.module.index][thatSource.indices.module.slot] != 0;
          }
    }
  }
}

template <bool Global>
void
FFModMatrixProcessor<Global>::ApplyModulation(
  FBModuleProcState& state)
{
  // perf
  if (_activeSlotCount == 0)
    return;

  FBAccParamState* targetParamState = nullptr;
  FBSArray<float, FBFixedBlockSamples> scaleOneBuffer = {};
  FBSArray<float, FBFixedBlockSamples> sourceOutBuffer = {};
  FBSArray<float, FBFixedBlockSamples> onNoteScaleBuffer = {};
  FBSArray<float, FBFixedBlockSamples> onNoteSourceBuffer = {};
  FBSArray<float, FBFixedBlockSamples> scaleMinMaxBuffer = {};
  FBSArray<float, FBFixedBlockSamples> scaledTargetAmtBuffer = {};
  FBSArray<float, FBFixedBlockSamples> const* scaleBuffer = nullptr;
  FBSArray<float, FBFixedBlockSamples> const* sourceBuffer = nullptr;
  FBSArray<float, FBFixedBlockSamples>* plugModulationBuffer = nullptr;

  auto* procState = state.ProcAs<FFProcState>();
  auto* procStateContainer = state.input->procState;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& procParams = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gMatrix[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vMatrix[state.moduleSlot]; });
  auto const& topo = state.topo->static_->modules[Global ? (int)FFModuleType::GMatrix : (int)FFModuleType::VMatrix];

  auto const& scaleMinNorm = procParams.acc.scaleMin;
  auto const& scaleMaxNorm = procParams.acc.scaleMax;
  auto const& targetAmtNorm = procParams.acc.targetAmt;
  auto const& sourceLowNorm = procParams.acc.sourceLow;
  auto const& sourceHighNorm = procParams.acc.sourceHigh;

  // static_cast for perf
  auto const& ffTopo = static_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& sources = Global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  auto const& targets = Global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI != nullptr)
  {
    auto& exchangeDSP = *FFSelectDualState<Global>(
      [exchangeToGUI, &state]() { return &exchangeToGUI->global.gMatrix[state.moduleSlot]; },
      [exchangeToGUI, &state, voice]() { return &exchangeToGUI->voice[voice].vMatrix[state.moduleSlot]; });
    exchangeDSP.boolIsActive = 1;
  }

  for (int i = 0; i < _activeSlotCount; i++)
  {
    auto const& scale = sources[_scale[i]];
    auto const& source = sources[_source[i]];
    auto const& target = targets[_target[i]];

    if constexpr (!Global)
    {
      // We can only fix the on-note values once the global 
      // mod source has cleared. Note to self: this caused
      // off-by-1-block error when we did this inside BeginVoice.
      if (_ownModSourceIsReadyForSlot[i])
      {
        if (!_onNoteWasSnapshotted[i])
        {
          _onNoteWasSnapshotted[i] = true;
          if (scale.onNote)
            _scaleOnNoteSnapshot[i] = GetSourceCVBuffer(state, scale, voice)->Get(state.voice->offsetInBlock);
          if (source.onNote)
            _sourceOnNoteSnapshot[i] = GetSourceCVBuffer(state, source, voice)->Get(state.voice->offsetInBlock);
        }
      }
    }

    if (_allModSourcesAreReadyForSlot[i] && !_slotHasBeenProcessed[i])
    {
      _slotHasBeenProcessed[i] = true;

      if(!source.onNote)
        sourceBuffer = GetSourceCVBuffer(state, source, voice);
      else 
      {
        onNoteSourceBuffer.Fill(_sourceOnNoteSnapshot[i]);
        sourceBuffer = &onNoteSourceBuffer;
      }

      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      {
        FBBatch<float> sourceLow = topo.NormalizedToIdentityFast((int)FFModMatrixParam::SourceLow, 
          FFSelectDualProcAccParamNormalized<Global>(sourceLowNorm[i], voice), s);
        FBBatch<float> sourceHigh = topo.NormalizedToIdentityFast((int)FFModMatrixParam::SourceHigh,
          FFSelectDualProcAccParamNormalized<Global>(sourceHighNorm[i], voice), s);
        FBBatch<float> sourceLowHigh = xsimd::clip(sourceBuffer->Load(s), sourceLow, sourceHigh);
        FBBoolBatch<float> highMinLowIsZero = (sourceHigh - sourceLow) == FBBatch<float>(0.0f);
        FBBatch<float> sourceLowHighOut = xsimd::clip((sourceLowHigh - sourceLow) / (sourceHigh - sourceLow), FBBatch<float>(0.0f), FBBatch<float>(1.0f));
        sourceOutBuffer.Store(s, xsimd::select(highMinLowIsZero, sourceLowHigh, sourceLowHighOut));
      }
        
      if (scale.indices.module.index == -1)
      {
        scaleOneBuffer.Fill(1.0f);
        scaleBuffer = &scaleOneBuffer;
      }
      else
      {
        if (!scale.onNote)
          scaleBuffer = GetSourceCVBuffer(state, scale, voice);
        else
        {
          onNoteScaleBuffer.Fill(_scaleOnNoteSnapshot[i]);
          scaleBuffer = &onNoteScaleBuffer;
        }
      }

      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      {
        FBBatch<float> scaleMin = topo.NormalizedToIdentityFast((int)FFModMatrixParam::ScaleMin,
          FFSelectDualProcAccParamNormalized<Global>(scaleMinNorm[i], voice), s);
        FBBatch<float> scaleMax = topo.NormalizedToIdentityFast((int)FFModMatrixParam::ScaleMax,
          FFSelectDualProcAccParamNormalized<Global>(scaleMaxNorm[i], voice), s);
        scaleMinMaxBuffer.Store(s, scaleMin + (scaleMax - scaleMin) * scaleBuffer->Load(s));
      }

      int runtimeTargetParamIndex = state.topo->audio.ParamAtTopo(target)->runtimeParamIndex;
      if constexpr (Global)
      {
        plugModulationBuffer = &procState->dsp.global.gMatrix.modulatedCV[i];
        targetParamState = &procStateContainer->Params()[runtimeTargetParamIndex].GlobalAcc().Global();
      }
      else
      {
        plugModulationBuffer = &procState->dsp.voice[voice].vMatrix.modulatedCV[i];
        targetParamState = &procStateContainer->Params()[runtimeTargetParamIndex].VoiceAcc().Voice()[voice];
      }

      auto const& targetAmt = FFSelectDualProcAccParamNormalized<Global>(targetAmtNorm[i], voice).CV();
      targetAmt.CopyTo(scaledTargetAmtBuffer);
      scaledTargetAmtBuffer.Mul(scaleMinMaxBuffer);
      targetParamState->CV().CopyTo(*plugModulationBuffer);

      FFApplyModulation(_opType[i], sourceOutBuffer, scaledTargetAmtBuffer, *plugModulationBuffer);
      targetParamState->ApplyPlugModulation(plugModulationBuffer);
      if (exchangeToGUI != nullptr)
      {
        auto& exchangeParams = *FFSelectDualState<Global>(
          [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gMatrix[state.moduleSlot]; },
          [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vMatrix[state.moduleSlot]; });

        FFSelectDualExchangeState<Global>(exchangeParams.acc.targetAmt[i], voice) = scaledTargetAmtBuffer.Last();
        FFSelectDualExchangeState<Global>(exchangeParams.acc.scaleMin[i], voice) = FFSelectDualProcAccParamNormalized<Global>(scaleMinNorm[i], voice).Last();
        FFSelectDualExchangeState<Global>(exchangeParams.acc.scaleMax[i], voice) = FFSelectDualProcAccParamNormalized<Global>(scaleMaxNorm[i], voice).Last();
        FFSelectDualExchangeState<Global>(exchangeParams.acc.sourceLow[i], voice) = FFSelectDualProcAccParamNormalized<Global>(sourceLowNorm[i], voice).Last();
        FFSelectDualExchangeState<Global>(exchangeParams.acc.sourceHigh[i], voice) = FFSelectDualProcAccParamNormalized<Global>(sourceHighNorm[i], voice).Last();
      }
    }
  }
}

template class FFModMatrixProcessor<true>;
template class FFModMatrixProcessor<false>;