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
  {
    _voiceState.fmOn[i] = topo.NormalizedToBoolFast(FFOsciModParam::FMOn, params.block.fmOn[i].Voice()[voice]);
    _voiceState.amMode[i] = topo.NormalizedToListFast<FFOsciModAMMode>(FFOsciModParam::AMMode, params.block.amMode[i].Voice()[voice]);
  }
}

void
FFOsciModProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& outputAMMix = procState->dsp.voice[voice].osciMod.outputAMMix;
  auto& outputFMIndex = procState->dsp.voice[voice].osciMod.outputFMIndex;
  auto const& procParams = procState->param.voice.osciMod[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciMod];

  // TODO these should themselves be mod targets
  // for now just copy over the stream
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto const& amMixNorm = procParams.acc.amMix[i].Voice()[voice];
    auto const& fmIndexNorm = procParams.acc.fmIndex[i].Voice()[voice];
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      if (_voiceState.amMode[i] != FFOsciModAMMode::Off)
        outputAMMix[i].Store(s, topo.NormalizedToIdentityFast(FFOsciModParam::AMMix, amMixNorm, s));
      if (_voiceState.fmOn[i])
        outputFMIndex[i].Store(s, topo.NormalizedToLog2Fast(FFOsciModParam::FMIndex, fmIndexNorm, s));
    }
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
    exchangeParams.acc.amMix[i][voice] = procParams.acc.amMix[i].Voice()[voice].Last();
    exchangeParams.acc.fmIndex[i][voice] = procParams.acc.fmIndex[i].Voice()[voice].Last();
  }
}