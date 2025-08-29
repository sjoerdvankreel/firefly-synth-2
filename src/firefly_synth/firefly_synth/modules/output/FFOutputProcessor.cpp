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
  float voicesNorm = state.input->voiceManager->VoiceCount() / (float)FBMaxVoices;
  _maxVoices = std::max(_maxVoices, voicesNorm);
  float cpuNorm = std::clamp(state.input->prevRoundCpuUsage, 0.0f, 1.0f);
  _maxCpu = std::max(_maxCpu, cpuNorm);
  float maxOutput = std::max(std::abs(output.audio[0].First()), std::abs(output.audio[1].First()));
  float gainNorm = std::clamp(maxOutput, 0.0f, 1.0f);
  _maxGain = std::max(_maxGain, gainNorm);

  auto now = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(now - _updated).count() < 1000.0f / OutputSamplingRate)
    return;

  if(_cpuParamIndex == -1)
  {
    _cpuParamIndex = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Cpu, 0 } })->runtimeParamIndex;
    _gainParamIndex = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Gain, 0 } })->runtimeParamIndex;
    _voicesParamIndex = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::Voices, 0 } })->runtimeParamIndex;
  }

  (*state.outputParamsNormalized)[_cpuParamIndex] = _maxCpu;
  (*state.outputParamsNormalized)[_gainParamIndex] = _maxGain;
  (*state.outputParamsNormalized)[_voicesParamIndex] = _maxVoices;

  _updated = now;
  _maxCpu = 0.0f;
  _maxGain = 0.0f;
  _maxVoices = 0.0f;
}