#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct EnvGraphRenderData final:
public FBModuleGraphRenderData<EnvGraphRenderData>
{
  FFEnvProcessor& GetProcessor(FBModuleProcState& state);
  int Process(FBModuleProcState& state) { return GetProcessor(state).Process(state); }
  void BeginVoice(FBModuleProcState& state) { GetProcessor(state).BeginVoice(state); }
};

FFEnvProcessor& 
EnvGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].env[state.moduleSlot].processor;
}

static FBModuleGraphPlotParams
PlotParams(FBGraphRenderState const* state)
{
  FBModuleGraphPlotParams result = {};
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float bpm = state->ExchangeContainer()->Host()->bpm;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  bool sync = state->AudioParamBool({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sync, 0 });
  
  if (!sync)
  {
    for(int i = 0; i < FFEnvStageCount; i++)
      result.samples += state->AudioParamLinearTimeSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageTime, i }, sampleRate);
    result.releaseAt = result.samples; // TODO
    return result;
  }

  for (int i = 0; i < FFEnvStageCount; i++)
    result.samples += state->AudioParamBarsSamples({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageBars, i }, sampleRate, bpm);
  result.releaseAt = result.samples; // TODO
  return result;
}

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  EnvGraphRenderData renderData = {};
  graphData->drawMarkers = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::Env;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].env[slot]; };
  renderData.voiceCVOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].env[slot].output; };
  FBTopoIndices indices = { (int)FFModuleType::Env, graphData->renderState->ModuleProcState()->moduleSlot };
  graphData->series[0].moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(indices)->name;
  FBRenderModuleGraph<false, false>(renderData, 0);
}