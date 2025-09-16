#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciModProcessor::BeginVoice(FBModuleProcState& state, bool graph)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osciMod[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::OsciMod];

  auto const& oversampleNorm = procParams.block.oversample[0].Voice()[voice];
  _oversample = !graph && topo.NormalizedToBoolFast(FFOsciModParam::Oversample, oversampleNorm);
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto const& fmOnNorm = procParams.block.fmOn[i].Voice()[voice];
    auto const& amModeNorm = procParams.block.amMode[i].Voice()[voice];
    _fmOn[i] = topo.NormalizedToBoolFast(FFOsciModParam::FMOn, fmOnNorm);
    _amMode[i] = topo.NormalizedToListFast<FFOsciModAMMode>(FFOsciModParam::AMMode, amModeNorm);
  }
}

void
FFOsciModProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& outputAMMix = voiceState.osciMod.outputAMMix;
  auto& outputFMIndex = voiceState.osciMod.outputFMIndex;
  auto const& procParams = procState->param.voice.osciMod[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::OsciMod];

  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto const& amMixNorm = procParams.acc.amMix[i].Voice()[voice];
    auto const& fmIndexNorm = procParams.acc.fmIndex[i].Voice()[voice];
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      if (_amMode[i] != FFOsciModAMMode::Off)
        outputAMMix[i].Store(s, topo.NormalizedToIdentityFast(FFOsciModParam::AMMix, amMixNorm, s));
      if (_fmOn[i])
        outputFMIndex[i].Store(s, topo.NormalizedToLog2Fast(FFOsciModParam::FMIndex, fmIndexNorm, s));
    }
    if (_oversample)
    {
      outputAMMix[i].UpsampleStretch<FFOsciOversampleTimes>();
      outputFMIndex[i].UpsampleStretch<FFOsciOversampleTimes>();
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeDSP = exchangeToGUI->voice[voice].osciMod[state.moduleSlot];
  exchangeDSP.active = true;
  auto& exchangeParams = exchangeToGUI->param.voice.osciMod[state.moduleSlot];
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    exchangeParams.acc.amMix[i][voice] = procParams.acc.amMix[i].Voice()[voice].Last();
    exchangeParams.acc.fmIndex[i][voice] = procParams.acc.fmIndex[i].Voice()[voice].Last();
  }
}