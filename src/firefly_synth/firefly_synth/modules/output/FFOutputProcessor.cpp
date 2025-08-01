#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void
FFOutputProcessor::Process(FBModuleProcState& state)
{
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Output];

  auto const* voicesParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Voices, 0 } });
  float voicesNorm = topo.DiscreteToNormalizedFast(FFOutputParam::Voices, state.input->voiceManager->VoiceCount());
  (*state.outputParamsNormalized)[voicesParam->runtimeParamIndex] = voicesNorm;

  auto now = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(now - _cpuUpdated).count() > 1000.0f / FFCpuSamplingRate)
  {
    auto const* cpuParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Cpu, 0 } });
    float cpuNorm = topo.DiscreteToNormalizedFast(FFOutputParam::Cpu, std::clamp((int)(state.input->prevRoundCpuUsage * FFMaxCpu), 0, FFMaxCpu));
    (*state.outputParamsNormalized)[cpuParam->runtimeParamIndex] = cpuNorm;
    _cpuUpdated = now;
  }
}