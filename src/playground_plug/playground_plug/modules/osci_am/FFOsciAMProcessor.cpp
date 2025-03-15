#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMProcessor.hpp>

#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciAMProcessor::BeginVoice(FBModuleProcState const& state)
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
}