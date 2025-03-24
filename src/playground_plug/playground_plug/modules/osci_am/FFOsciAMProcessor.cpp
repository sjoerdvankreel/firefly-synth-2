#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMProcessor.hpp>

#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciAMProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osciAM[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciAM];
  for(int i = 0; i < FFOsciModSlotCount; i++)
    _voiceState.on[i] = topo.NormalizedToBoolFast(FFOsciAMParam::On, params.block.on[i].Voice()[voice]);
}

void
FFOsciAMProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& outputMix = procState->dsp.voice[voice].osciAM.outputMix;
  auto& outputRing = procState->dsp.voice[voice].osciAM.outputRing;
  auto const& procParams = procState->param.voice.osciAM[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciAM];

  // TODO these should themselves be mod targets
  // for now just copy over the stream
  for (int i = 0; i < FFOsciModSlotCount; i++)
    if(_voiceState.on[i])
    {
      auto const& mixNorm = procParams.acc.mix[i].Voice()[voice];
      auto const& ringNorm = procParams.acc.ring[i].Voice()[voice];
      topo.NormalizedToIdentityFast(FFOsciAMParam::Mix, mixNorm, outputMix[i]);
      topo.NormalizedToIdentityFast(FFOsciAMParam::Ring, ringNorm, outputRing[i]);
    }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osciAM[state.moduleSlot];
  exchangeDSP.active = true;

  // TODO accurately reflect outputMix/Ring
  auto& exchangeParams = exchangeToGUI->param.voice.osciAM[state.moduleSlot];
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    exchangeParams.acc.mix[i][voice] = procParams.acc.mix[i].Voice()[voice].Last();
    exchangeParams.acc.ring[i][voice] = procParams.acc.ring[i].Voice()[voice].Last();
  }
}