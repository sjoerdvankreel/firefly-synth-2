#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void
FFOutputProcessor::Process(FBModuleProcState& state, FBPlugOutputBlock const& output)
{
  auto now = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(now - _updated).count() < 1000.0f / FFOutputSamplingRate)
    return;
  _updated = now;

  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Output];

  auto const* voicesParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Voices, 0 } });
  float voicesNorm = topo.DiscreteToNormalizedFast(FFOutputParam::Voices, state.input->voiceManager->VoiceCount());
  (*state.outputParamsNormalized)[voicesParam->runtimeParamIndex] = voicesNorm;

  auto const* cpuParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Cpu, 0 } });
  float cpuNorm = topo.DiscreteToNormalizedFast(FFOutputParam::Cpu, std::clamp((int)(state.input->prevRoundCpuUsage * FFOutputMaxCpu), 0, FFOutputMaxCpu));
  (*state.outputParamsNormalized)[cpuParam->runtimeParamIndex] = cpuNorm;

  float maxOutput = std::max(std::abs(output.audio[0].First()), std::abs(output.audio[1].First()));
  auto const* gainParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Gain, 0 } });
  float gainNorm = topo.DiscreteToNormalizedFast(FFOutputParam::Gain, std::clamp((int)(maxOutput * FFOutputMaxGain), 0, FFOutputMaxGain));
  (*state.outputParamsNormalized)[gainParam->runtimeParamIndex] = gainNorm;
}