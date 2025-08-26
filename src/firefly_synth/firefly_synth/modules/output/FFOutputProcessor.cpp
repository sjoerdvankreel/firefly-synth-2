#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

static int constexpr OutputSamplingRate = 10;

void
FFOutputProcessor::Process(FBModuleProcState& state, FBPlugOutputBlock const& output)
{
  (void)state;
  (void)output;
#if 0//TODO SLOOOOOW

  auto const* voicesParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Voices, 0 } });
  float voicesNorm = state.input->voiceManager->VoiceCount() / (float)FBMaxVoices;
  _maxVoices = std::max(_maxVoices, voicesNorm);

  auto const* cpuParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Cpu, 0 } });
  float cpuNorm = std::clamp(state.input->prevRoundCpuUsage, 0.0f, 1.0f);
  _maxCpu = std::max(_maxCpu, cpuNorm);

  float maxOutput = std::max(std::abs(output.audio[0].First()), std::abs(output.audio[1].First()));
  auto const* gainParam = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Gain, 0 } });
  float gainNorm = std::clamp(maxOutput, 0.0f, 1.0f);
  _maxGain = std::max(_maxGain, gainNorm);

  auto now = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(now - _updated).count() < 1000.0f / OutputSamplingRate)
    return;
  (*state.outputParamsNormalized)[voicesParam->runtimeParamIndex] = _maxVoices;
  (*state.outputParamsNormalized)[cpuParam->runtimeParamIndex] = _maxCpu;
  (*state.outputParamsNormalized)[gainParam->runtimeParamIndex] = _maxGain;

  _updated = now;
  _maxCpu = 0.0f;
  _maxGain = 0.0f;
  _maxVoices = 0.0f;
#endif
}