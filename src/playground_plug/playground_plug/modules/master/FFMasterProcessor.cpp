#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

void
FFMasterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.master.output;
  auto const& input = procState->dsp.global.master.input;
  auto const& procParams = procState->param.global.master[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Master];

  FBFixedFloatBlock gainPlain;
  auto const& gainNorm = procParams.acc.gain[0].Global();
  topo.NormalizedToIdentityFast(FFMasterParam::Gain, gainNorm, gainPlain);
  output.Transform([&](int ch, int v) { return input[ch][v] * gainPlain[v]; });

  auto const* voicesParam = state.topo->audio.ParamAtTopo({ (int)FFModuleType::Master, 0, (int)FFMasterParam::Voices, 0 });
  float voicesNorm = topo.DiscreteToNormalizedFast(FFMasterParam::Voices, state.input->voiceManager->VoiceCount());
  (*state.outputParamsNormalized)[voicesParam->runtimeParamIndex] = voicesNorm;

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeDSP = exchangeToGUI->global.master[state.moduleSlot];
  exchangeDSP.active = true;
  auto& exchangeParams = exchangeToGUI->param.global.master[state.moduleSlot];
  exchangeParams.acc.gain[0] = gainNorm.Last();
}