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
  if (state.topo->static_->modules[mi].voice)
    return sourceCvOutput.voiceAddr(ms, os, voice, procState);
  else
    return sourceCvOutput.globalAddr(ms, os, procState);
}

template <bool Global>
void
FFModMatrixProcessor<Global>::InitializeBuffers(FBRuntimeTopo const* topo)
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
  // On ApplyModulation, we can't do it right away.
  // For each slot, need to wait untill all slots with the same
  // target param have cleared their modsource, and then apply.
  // Otherwise, the stacked modes are stacking against something that has yet to come.
  // Scale is ok because it can never be "later" than the modsource.
  for (int i = 0; i < _modSourceIsReady.size(); i++)
    for (int j = 0; j < _modSourceIsReady[i].size(); j++)
      _modSourceIsReady[i][j] = 0;
  for (int i = 0; i < SlotCount; i++)
  {
    _slotHasBeenProcessed[i] = false;
    _allModSourcesAreReadyForSlot[i] = false;
  }
}

template <bool Global>
void 
FFModMatrixProcessor<Global>::EndModulationBlock(FBModuleProcState& state)
{
  auto* procStateContainer = state.input->procState;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  // static_cast for perf
  auto const& ffTopo = static_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& targets = Global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;
  for (int i = 0; i < SlotCount; i++)
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
  auto const& amountNorm = params.acc.amount;

  for (int i = 0; i < SlotCount; i++)
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
    _opType[i] = topo.NormalizedToListFast<FFModMatrixOpType>(
      FFModMatrixParam::OpType,
      FFSelectDualProcBlockParamNormalized<Global>(opTypeNorm[i], voice));
  }

  // snapshot on-note values
  if constexpr (!Global)
  {
    // static_cast for perf
    auto const& ffTopo = static_cast<FFStaticTopo const&>(*state.topo->static_);
    auto const& sources = ffTopo.vMatrixSources;
    for (int i = 0; i < SlotCount; i++)
    {
      if (_opType[i] != FFModMatrixOpType::Off)
      {
        auto const& scale = sources[_scale[i]];
        if (scale.onNote)
          _scaleOnNoteValues[i] = GetSourceCVBuffer(state, scale, voice)->Get(state.voice->offsetInBlock);
        auto const& source = sources[_source[i]];
        if (source.onNote)
          _sourceOnNoteValues[i] = GetSourceCVBuffer(state, source, voice)->Get(state.voice->offsetInBlock);
      }
    }
  }

  // Prevent unscaled amount from showing up as 0.
  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI != nullptr)
  {
    auto& exchangeParams = *FFSelectDualState<Global>(
      [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gMatrix[state.moduleSlot]; },
      [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vMatrix[state.moduleSlot]; });
    for (int i = 0; i < SlotCount; i++)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.amount[i], voice) =
      FFSelectDualProcAccParamNormalized<Global>(amountNorm[i], voice).CV().Get(0);
  }
}

template <bool Global>
void
FFModMatrixProcessor<Global>::ApplyModulation(
  FBModuleProcState& state, FBTopoIndices const& currentModule)
{
  FBAccParamState* targetParamState = nullptr;
  FBSArray<float, FBFixedBlockSamples> onNoteScaleBuffer = {};
  FBSArray<float, FBFixedBlockSamples> onNoteSourceBuffer = {};
  FBSArray<float, FBFixedBlockSamples> scaledSourceBuffer = {};
  FBSArray<float, FBFixedBlockSamples> scaledAmountBuffer = {};
  FBSArray<float, FBFixedBlockSamples> const* scaleBuffer = nullptr;
  FBSArray<float, FBFixedBlockSamples> const* sourceBuffer = nullptr;
  FBSArray<float, FBFixedBlockSamples>* plugModulationBuffer = nullptr;

  auto* procState = state.ProcAs<FFProcState>();
  auto* procStateContainer = state.input->procState;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& procParams = *FFSelectDualState<Global>(
    [procState, &state]() { return &procState->param.global.gMatrix[state.moduleSlot]; },
    [procState, &state]() { return &procState->param.voice.vMatrix[state.moduleSlot]; });

  auto const& amountNorm = procParams.acc.amount;
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
    exchangeDSP.active = true;
  }

  _modSourceIsReady[currentModule.index][currentModule.slot] = 1;

  // need all slots with same target ready before we begin processing
  for (int i = 0; i < SlotCount; i++)
  {
    _allModSourcesAreReadyForSlot[i] = _opType[i] != FFModMatrixOpType::Off;
    if (_allModSourcesAreReadyForSlot[i])
    {
      auto const& thisTarget = targets[_target[i]];
      auto const& thisSource = sources[_source[i]];
      _allModSourcesAreReadyForSlot[i] &= thisTarget.module.index != -1;
      _allModSourcesAreReadyForSlot[i] &= thisSource.indices.module.index != -1;
      if (_allModSourcesAreReadyForSlot[i])
        for (int j = 0; j < SlotCount; j++)
          if (_opType[i] != FFModMatrixOpType::Off)
          {
            auto const& thatSource = sources[_source[j]];
            auto const& thatTarget = targets[_target[j]];
            if (thatSource.indices.module.index != -1 && thatTarget.module == thisTarget.module)
              _allModSourcesAreReadyForSlot[i] &= _modSourceIsReady[thatSource.indices.module.index][thatSource.indices.module.slot] != 0;
          }
    }
  }

  for (int i = 0; i < SlotCount; i++)
  {
    if (_allModSourcesAreReadyForSlot[i] && !_slotHasBeenProcessed[i])
    {
      auto const& scale = sources[_scale[i]];
      auto const& source = sources[_source[i]];
      auto const& target = targets[_target[i]];
      _slotHasBeenProcessed[i] = true;

      if(!source.onNote)
        sourceBuffer = GetSourceCVBuffer(state, source, voice);
      else 
      {
        onNoteSourceBuffer.Fill(_sourceOnNoteValues[i]);
        sourceBuffer = &onNoteSourceBuffer;
      }
        
      scaleBuffer = nullptr;
      if(scale.indices.module.index != -1)
        if (!scale.onNote)
          scaleBuffer = GetSourceCVBuffer(state, scale, voice);
        else
        {
          onNoteScaleBuffer.Fill(_scaleOnNoteValues[i]);
          scaleBuffer = &onNoteScaleBuffer;
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

      auto const& amount = FFSelectDualProcAccParamNormalized<Global>(amountNorm[i], voice).CV();
      amount.CopyTo(scaledAmountBuffer);
      if (scaleBuffer != nullptr)
        scaledAmountBuffer.Mul(*scaleBuffer);
      sourceBuffer->CopyTo(scaledSourceBuffer);
      targetParamState->CV().CopyTo(*plugModulationBuffer);

      switch (_opType[i])
      {
      case FFModMatrixOpType::Mul:
        for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
          plugModulationBuffer->Store(s, 
            (1.0f - scaledAmountBuffer.Load(s)) * plugModulationBuffer->Load(s) + 
            scaledAmountBuffer.Load(s) * scaledSourceBuffer.Load(s) * plugModulationBuffer->Load(s));
        break;
      case FFModMatrixOpType::Add:
        scaledSourceBuffer.Mul(scaledAmountBuffer);
        plugModulationBuffer->Add(scaledSourceBuffer);
        for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
          plugModulationBuffer->Store(s, xsimd::clip(plugModulationBuffer->Load(s), FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
        break;
      case FFModMatrixOpType::Stack:
        scaledSourceBuffer.Mul(scaledAmountBuffer);
        for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
          plugModulationBuffer->Add(s, (1.0f - plugModulationBuffer->Load(s)) * scaledSourceBuffer.Load(s));
        break;
      case FFModMatrixOpType::BPAdd:
        for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        {
          auto bpScaledSource = FBToBipolar(scaledSourceBuffer.Load(s));
          bpScaledSource *= scaledAmountBuffer.Load(s);
          auto bpScaledTarget = FBToBipolar(plugModulationBuffer->Load(s));
          bpScaledTarget += bpScaledSource;
          bpScaledTarget = xsimd::clip(bpScaledTarget, FBBatch<float>(-1.0f), FBBatch<float>(1.0f));
          plugModulationBuffer->Store(s, FBToUnipolar(bpScaledTarget));
        }
        break;
      case FFModMatrixOpType::BPStack:
        for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        {
          auto bpScaledSource = FBToBipolar(scaledSourceBuffer.Load(s));
          bpScaledSource *= scaledAmountBuffer.Load(s);
          auto bpScaledTarget = FBToBipolar(plugModulationBuffer->Load(s));
          auto headroom = 1.0f - xsimd::abs(bpScaledTarget);
          bpScaledTarget += bpScaledSource * headroom;
          plugModulationBuffer->Store(s, FBToUnipolar(bpScaledTarget));
        }
        break;
      default:
        FB_ASSERT(false);
        break;
      }

      targetParamState->ApplyPlugModulation(plugModulationBuffer);

      if (exchangeToGUI != nullptr)
      {
        auto& exchangeParams = *FFSelectDualState<Global>(
          [exchangeToGUI, &state] { return &exchangeToGUI->param.global.gMatrix[state.moduleSlot]; },
          [exchangeToGUI, &state] { return &exchangeToGUI->param.voice.vMatrix[state.moduleSlot]; });
        FFSelectDualExchangeState<Global>(exchangeParams.acc.amount[i], voice) = scaledAmountBuffer.Last();
      }
    }
  }
}

template class FFModMatrixProcessor<true>;
template class FFModMatrixProcessor<false>;