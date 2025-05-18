#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseGraph.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct KSNoiseGraphRenderData final :
public FBModuleGraphRenderData<KSNoiseGraphRenderData>
{
  FFKSNoiseProcessor& GetProcessor(FBModuleProcState& state);
  int Process(FBModuleProcState& state) { return GetProcessor(state).Process(state); }
  void BeginVoice(FBModuleProcState& state) { GetProcessor(state).BeginVoice(state); }
};

FFKSNoiseProcessor&
KSNoiseGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& processor = *procState->dsp.voice[state.voice->slot].ksNoise[state.moduleSlot].processor;
  processor.AllocateBuffers(state.input->sampleRate);
  return processor;
}

static FBModuleGraphPlotParams
PlotParams(FBGraphRenderState const* state)
{
  FBModuleGraphPlotParams result = {};
  result.releaseAt = -1;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear({ (int)FFModuleType::KSNoise, moduleSlot, (int)FFKSNoiseParam::Coarse, 0 }));
  pitch += state->AudioParamLinear({ (int)FFModuleType::KSNoise, moduleSlot, (int)FFKSNoiseParam::Fine, 0 });
  result.samples = FBFreqToSamples(FBPitchToFreq(pitch), sampleRate) * FFKSNoiseGraphRounds;
  result.samples = 20000; // todo
  return result;
}

void
FFKSNoiseRenderGraph(FBModuleGraphComponentData* graphData)
{
  KSNoiseGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::KSNoise;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].ksNoise[slot]; };
  renderData.voiceAudioOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].ksNoise[slot].output; };
  FBTopoIndices indices = { (int)FFModuleType::KSNoise, graphData->renderState->ModuleProcState()->moduleSlot };
  graphData->series[0].moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(indices)->name;
  FBRenderModuleGraph<false, true>(renderData, 0);
}