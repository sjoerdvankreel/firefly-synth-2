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
FFModMatrixProcessor<Global>::ClearModulation(FBModuleProcState& state)
{
  auto* procStateContainer = state.input->procState;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*state.topo->static_);
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

  if constexpr (!Global)
  {
    auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*state.topo->static_);
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
}

template <bool Global>
void
FFModMatrixProcessor<Global>::ApplyModulation(
  FBModuleProcState& state, FBTopoIndices const& currentModule)
{
  // TODO no need to make this O(N^whatever)
  // todo validate if possible
  // todo scale
  // todo amount
  // todo not only add

  FBAccParamState* targetParamState = nullptr;
  FBSArray<float, FBFixedBlockSamples> onNoteScaleBuffer = {};
  FBSArray<float, FBFixedBlockSamples> onNoteSourceBuffer = {};
  FBSArray<float, FBFixedBlockSamples> scaledSourceBuffer = {};
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
  auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& sources = Global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  auto const& targets = Global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;

  for (int i = 0; i < SlotCount; i++)
  {
    if (_opType[i] != FFModMatrixOpType::Off)
    {
      auto const& scale = sources[_scale[i]];
      auto const& source = sources[_source[i]];
      auto const& target = targets[_target[i]];
      if (source.indices.module == currentModule && target.module.index != -1)
      {
        if(!source.onNote)
          sourceBuffer = GetSourceCVBuffer(state, source, voice);
        else 
        {
          onNoteSourceBuffer.Fill(_sourceOnNoteValues[i]);
          sourceBuffer = &onNoteSourceBuffer;
        }
        
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
        sourceBuffer->CopyTo(scaledSourceBuffer);
        scaledSourceBuffer.Mul(amount);
        if (scaleBuffer != nullptr)
          scaledSourceBuffer.Mul(*scaleBuffer);
        targetParamState->CV().CopyTo(*plugModulationBuffer);
        plugModulationBuffer->Mul(scaledSourceBuffer);
        targetParamState->ApplyPlugModulation(plugModulationBuffer);
      }
    }
  }
}

template class FFModMatrixProcessor<true>;
template class FFModMatrixProcessor<false>;