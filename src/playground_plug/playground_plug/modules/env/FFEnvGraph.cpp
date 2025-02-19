#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

static int
PlotLengthSamples(FBGraphRenderState const* state)
{
  int result = 0;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float bpm = state->ExchangeContainer()->Proc()->bpm;
  float sampleRate = state->ExchangeContainer()->Proc()->sampleRate;
  bool sync = state->AudioParamBool({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sync, 0 });
  if (!sync)
  {
    result += state->AudioParamLog2TimeSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::HoldTime, 0 }, sampleRate);
    result += state->AudioParamLog2TimeSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecayTime, 0 }, sampleRate);
    result += state->AudioParamLog2TimeSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DelayTime, 0 }, sampleRate);
    result += state->AudioParamLog2TimeSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackTime, 0 }, sampleRate);
    result += state->AudioParamLog2TimeSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothTime, 0 }, sampleRate);
    result += state->AudioParamLog2TimeSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseTime, 0 }, sampleRate);
    return result;
  }

  result += state->AudioParamBarsSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::HoldBars, 0 }, sampleRate, bpm);
  result += state->AudioParamBarsSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecayBars, 0 }, sampleRate, bpm);
  result += state->AudioParamBarsSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DelayBars, 0 }, sampleRate, bpm);
  result += state->AudioParamBarsSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackBars, 0 }, sampleRate, bpm);
  result += state->AudioParamBarsSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothBars, 0 }, sampleRate, bpm);
  result += state->AudioParamBarsSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseBars, 0 }, sampleRate, bpm);
  return result;
}

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  FBModuleGraphRenderData<FFEnvProcessor> renderData = {};
  renderData.graphData = graphData;
  renderData.staticModuleIndex = (int)FFModuleType::Env;
  renderData.plotLengthSelector = PlotLengthSamples;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].env[slot]; };
  renderData.voiceOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].env[slot].output; };
  FBRenderModuleGraph<false>(renderData);
}