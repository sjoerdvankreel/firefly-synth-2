#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

void
FFMasterProcessor::Process(FBModuleProcState const& state)
{
  auto* procState = state.ProcState<FFProcState>();
  auto& output = procState->dsp.global.master.output;
  auto const& input = procState->dsp.global.master.input;
  auto const& params = procState->param.global.master[state.moduleSlot];

  auto const& gain = params.acc.gain[0].Global();
  output.Transform([&](int ch, int v) { return input[ch][v] * gain.CV(v); });
  auto const* voicesParam = state.topo->ParamAtTopo({ (int)FFModuleType::Master, 0, (int)FFMasterParam::Voices, 0 });
  float voicesNorm = voicesParam->static_.discrete.PlainToNormalized(state.input->voiceManager->VoiceCount());
  (*state.outputParamsNormalized)[voicesParam->runtimeParamIndex] = voicesNorm;

  auto* exchangeState = state.ExchangeState<FFExchangeState>();
  if (exchangeState == nullptr)
    return;
  exchangeState->global.master[0].active = true;
  auto& exchangeParams = exchangeState->param.global.master[0];
  exchangeParams.acc.gain[0] = gain.CV().data[FBFixedBlockSamples - 1];
}