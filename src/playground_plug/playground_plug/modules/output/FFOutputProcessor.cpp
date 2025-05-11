#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/output/FFOutputTopo.hpp>
#include <playground_plug/modules/output/FFOutputProcessor.hpp>

#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void
FFOutputProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.global.output[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Output];

  auto const* voicesParam = state.topo->audio.ParamAtTopo({ (int)FFModuleType::Output, 0, (int)FFOutputParam::Voices, 0 });
  float voicesNorm = topo.DiscreteToNormalizedFast(FFOutputParam::Voices, state.input->voiceManager->VoiceCount());
  (*state.outputParamsNormalized)[voicesParam->runtimeParamIndex] = voicesNorm;
}