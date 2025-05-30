#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/physical/FFPhysGraph.hpp>
#include <playground_plug/modules/physical/FFPhysProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct PhysGraphRenderData final :
public FBModuleGraphRenderData<PhysGraphRenderData>
{
  FFPhysProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBModuleProcState& state, int graphIndex) { return GetProcessor(state).Process(state); }
  void DoBeginVoice(FBModuleProcState& state, int graphIndex) { GetProcessor(state).BeginVoice(true, state); }
  void DoProcessIndicators(bool exchange, int exchangeVoice, int graphIndex, FBModuleGraphPoints& points) {}
};

FFPhysProcessor&
PhysGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& processor = *procState->dsp.voice[state.voice->slot].phys[state.moduleSlot].processor;
  processor.Initialize(true, state.input->sampleRate);
  return processor;
}

static int
PlotSamples(FBModuleGraphComponentData const* data)
{
  auto const* state = data->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Coarse, 0 }, false, -1));
  pitch += state->AudioParamLinear({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Fine, 0 }, false, -1);
  return FBFreqToSamples(FBPitchToFreq(pitch), sampleRate) * FFPhysGraphRounds;
}

void
FFPhysRenderGraph(FBModuleGraphComponentData* graphData)
{
  PhysGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false;
  renderData.graphData = graphData;
  renderData.plotSamplesSelector = PlotSamples;
  renderData.staticModuleIndex = (int)FFModuleType::Phys;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].phys[slot]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].phys[slot].output; };
  FBTopoIndices indices = { (int)FFModuleType::Phys, graphData->renderState->ModuleProcState()->moduleSlot };
  graphData->graphs[0].moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(indices)->name;
  FBRenderModuleGraph<false, true>(renderData, 0);
}