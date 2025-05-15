#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/noise/FFNoiseGraph.hpp>
#include <playground_plug/modules/noise/FFNoiseProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct NoiseGraphRenderData final :
public FBModuleGraphRenderData<NoiseGraphRenderData>
{
  FFNoiseProcessor& GetProcessor(FBModuleProcState& state);
  int Process(FBModuleProcState& state) { return GetProcessor(state).Process(state); }
  void BeginVoice(FBModuleProcState& state) { GetProcessor(state).BeginVoice(state); }
};

FFNoiseProcessor&
NoiseGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].noise[state.moduleSlot].processor;
}

static FBModuleGraphPlotParams
PlotParams(FBGraphRenderState const* state)
{
  FBModuleGraphPlotParams result = {};
  result.releaseAt = -1;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Coarse, 0 }));
  pitch += state->AudioParamLinear({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Fine, 0 });
  result.samples = FBFreqToSamples(FBPitchToFreq(pitch), sampleRate);
  return result;
}

void
FFNoiseRenderGraph(FBModuleGraphComponentData* graphData)
{
  NoiseGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::Noise;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].noise[slot]; };
  renderData.voiceAudioOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].noise[slot].output; };
  FBTopoIndices indices = { (int)FFModuleType::Noise, graphData->renderState->ModuleProcState()->moduleSlot };
  graphData->series[0].moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(indices)->name;
  FBRenderModuleGraph<false, true>(renderData, 0);
}