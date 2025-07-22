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
  // todo not only add
  // todo not overwrite external modulation
  auto* procStateContainer = state.input->procStateContainer;
  auto const& ffTopo = dynamic_cast<FFStaticTopo const&>(*state.topo->static_);
  auto const& sources = Global ? ffTopo.gMatrixSources : ffTopo.vMatrixSources;
  auto const& targets = Global ? ffTopo.gMatrixTargets : ffTopo.vMatrixTargets;
  for (int i = 0; i < SlotCount; i++)
  {
    if (_opType[i] != FFModMatrixOpType::Off)
    {
      auto const& source = sources[_source[i]];
      if (source.indices.module == currentModule)
      {
        auto const& target = targets[_target[i]];
        int runtimeTargetParamIndex = state.topo->audio.ParamAtTopo(target)->runtimeParamIndex;
        procStateContainer->foo();
      }
    }
  }
}

template class FFModMatrixProcessor<true>;
template class FFModMatrixProcessor<false>;