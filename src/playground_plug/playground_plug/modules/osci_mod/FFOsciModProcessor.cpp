#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModProcessor.hpp>

#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciModProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osciMod[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciMod];
  for (int i = 0; i < FFOsciModSlotCount; i++)
    _voiceState.on[i] = topo.NormalizedToBoolFast(FFOsciModParam::On, params.block.on[i].Voice()[voice]);
}

void
FFOsciModProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& outputAM = procState->dsp.voice[voice].osciMod.outputAM;
  auto& outputRM = procState->dsp.voice[voice].osciMod.outputRM;
  auto& outputFM = procState->dsp.voice[voice].osciMod.outputFM;
  auto const& procParams = procState->param.voice.osciMod[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciMod];

  // TODO these should themselves be mod targets
  // for now just copy over the stream
  for (int i = 0; i < FFOsciModSlotCount; i++)
    if(_voiceState.on[i])
    {
      auto const& amNorm = procParams.acc.am[i].Voice()[voice];
      auto const& rmNorm = procParams.acc.rm[i].Voice()[voice];
      auto const& fmNorm = procParams.acc.fm[i].Voice()[voice];
      topo.NormalizedToLog2Fast(FFOsciModParam::FM, fmNorm, outputFM[i]);
      topo.NormalizedToIdentityFast(FFOsciModParam::AM, amNorm, outputAM[i]);
      topo.NormalizedToIdentityFast(FFOsciModParam::RM, rmNorm, outputRM[i]);
    }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osciMod[state.moduleSlot];
  exchangeDSP.active = true;

  // TODO accurately reflect output
  auto& exchangeParams = exchangeToGUI->param.voice.osciMod[state.moduleSlot];
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    exchangeParams.acc.am[i][voice] = procParams.acc.am[i].Voice()[voice].Last();
    exchangeParams.acc.rm[i][voice] = procParams.acc.rm[i].Voice()[voice].Last();
    exchangeParams.acc.fm[i][voice] = procParams.acc.fm[i].Voice()[voice].Last();
  }
}