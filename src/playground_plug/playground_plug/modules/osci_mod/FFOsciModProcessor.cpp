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
  auto const& procParams = procState->param.voice.osciMod[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciMod];

  auto const& oversamplingNorm = procParams.block.oversampling[0].Voice()[voice];
  bool oversampling = topo.NormalizedToBoolFast(FFOsciModParam::Oversampling, oversamplingNorm);
  _oversamplingTimes = oversampling ? FFOsciOversamplingTimes : 1;
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
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::OsciMod];
  
  int stepSamples = FBSIMDFloatCount * _oversamplingTimes;
  int totalSamples = FBFixedBlockSamples * _oversamplingTimes;
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto const& amMixNorm = procParams.acc.amMix[i].Voice()[voice];
    auto const& fmIndexNorm = procParams.acc.fmIndex[i].Voice()[voice];
    for (int s = 0; s < totalSamples; s += stepSamples)
    {
      if (_amMode[i] != FFOsciModAMMode::Off)
      {
        auto amMixPlain = topo.NormalizedToIdentityFast(FFOsciModParam::AMMix, amMixNorm, s);
        outputAMMix[i].StoreRepeat(s, _oversamplingTimes, amMixPlain);
      }
      if (_fmOn[i])
      {
        auto fmIndexPlain = topo.NormalizedToLog2Fast(FFOsciModParam::FMIndex, fmIndexNorm, s);
        outputFMIndex[i].StoreRepeat(s, _oversamplingTimes, fmIndexPlain);
      }
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