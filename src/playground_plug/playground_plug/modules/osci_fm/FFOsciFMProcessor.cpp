#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMProcessor.hpp>

#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciFMProcessor::BeginVoice(FBModuleProcState const& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osciFM[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciFM];
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    _voiceState.on[i] = topo.NormalizedToBoolFast(FFOsciFMParam::On, params.block.on[i].Voice()[voice]);
    _voiceState.throughZero[i] = topo.NormalizedToBoolFast(FFOsciFMParam::ThroughZero, params.block.throughZero[i].Voice()[voice]);
  }
}

void
FFOsciFMProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& outputIndex = procState->dsp.voice[voice].osciFM.outputIndex;
  auto const& procParams = procState->param.voice.osciFM[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciFM];

  // TODO these should themselves be mod targets
  // for now just copy over the stream
  for (int i = 0; i < FFOsciModSlotCount; i++)
    if(_voiceState.on[i])
    {
      // todo not identity
      auto const& indexNorm = procParams.acc.index[i].Voice()[voice];
      topo.NormalizedToIdentityFast(FFOsciFMParam::Index, indexNorm, outputIndex[i]);
    }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osciFM[state.moduleSlot];
  exchangeDSP.active = true;

  // TODO accurately reflect outputIndex
  auto& exchangeParams = exchangeToGUI->param.voice.osciFM[state.moduleSlot];
  for (int i = 0; i < FFOsciModSlotCount; i++)
    exchangeParams.acc.index[i][voice] = procParams.acc.index[i].Voice()[voice].Last();
}