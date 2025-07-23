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

template <bool Global>
void
FFModMatrixProcessor<Global>::InitializeBuffers(FBProcStateContainer* container)
{
  _allPossibleTargetParams.clear();
  for (int i = 0; i < container->Params().size(); i++)
    if (container->Params()[i].IsAcc())
      if constexpr (Global)
      {
        if (!container->Params()[i].IsVoice())
          _allPossibleTargetParams.push_back(i);
      }
      else
      {
        if (container->Params()[i].IsVoice())
          _allPossibleTargetParams.push_back(i);
      }
}

template <bool Global>
void 
FFModMatrixProcessor<Global>::ClearModulation(FBModuleProcState& state)
{
  auto* procStateContainer = state.input->procState;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  for(int i = 0; i < _allPossibleTargetParams.size(); i++)
    if constexpr(Global)
      procStateContainer->Params()[_allPossibleTargetParams[i]].GlobalAcc().Global().ClearPlugModulation();
    else
      procStateContainer->Params()[_allPossibleTargetParams[i]].VoiceAcc().Voice()[voice].ClearPlugModulation();
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
}

template <bool Global>
void
FFModMatrixProcessor<Global>::ApplyModulation(
  FBModuleProcState& state, FBTopoIndices const& currentModule)
{
  // TODO no need to make this O(N^whatever)
  // todo on note
  // todo validate if possible
  // todo scale
  // todo amount
  // todo not only add

  FBAccParamState* targetParamState = nullptr;
  FBSArray<float, FBFixedBlockSamples> const* sourceBuffer = nullptr;
  FBSArray<float, FBFixedBlockSamples>* plugModulationBuffer = nullptr;

  auto* procState = state.ProcAs<FFProcState>();
  auto* procStateContainer = state.input->procState;
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& sources = Global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  auto const& targets = Global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;

  for (int i = 0; i < SlotCount; i++)
  {
    if (_opType[i] != FFModMatrixOpType::Off)
    {
      auto const& source = sources[_source[i]];
      auto const& target = targets[_target[i]];
      if (source.indices.module == currentModule && target.module.index != -1)
      {
        int sms = source.indices.module.slot;
        int smi = source.indices.module.index;
        int sos = source.indices.cvOutput.slot;
        int soi = source.indices.cvOutput.index;
        auto const& sourceCvOutput = state.topo->static_->modules[smi].cvOutputs[soi];
        int runtimeTargetParamIndex = state.topo->audio.ParamAtTopo(target)->runtimeParamIndex;
        if constexpr (Global)
        {
          sourceBuffer = sourceCvOutput.globalAddr(sms, sos, procState);
          plugModulationBuffer = &procState->dsp.global.gMatrix.modulatedCV[i];
          targetParamState = &procStateContainer->Params()[runtimeTargetParamIndex].GlobalAcc().Global();
        }
        else
        {
          sourceBuffer = sourceCvOutput.voiceAddr(sms, sos, voice, procState);
          plugModulationBuffer = &procState->dsp.voice[voice].vMatrix.modulatedCV[i];
          targetParamState = &procStateContainer->Params()[runtimeTargetParamIndex].VoiceAcc().Voice()[voice];
        }
        targetParamState->CV().CopyTo(*plugModulationBuffer);
        plugModulationBuffer->Mul(*sourceBuffer);
        targetParamState->ApplyPlugModulation(plugModulationBuffer);
      }
    }
  }
}

template class FFModMatrixProcessor<true>;
template class FFModMatrixProcessor<false>;