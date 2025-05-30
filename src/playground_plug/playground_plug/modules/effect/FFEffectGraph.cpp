#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/effect/FFEffectProcessor.hpp>
#include <playground_plug/modules/effect/FFEffectGraph.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct EffectGraphRenderData final:
public FBModuleGraphRenderData<EffectGraphRenderData>
{
  FFEffectProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBModuleProcState& state) { GetProcessor(state).Process(state); return 16; }
  void DoBeginVoice(FBModuleProcState& state) { GetProcessor(state).BeginVoice(state); }
  void DoProcessIndicators(bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

FFEffectProcessor&
EffectGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].effect[state.moduleSlot].processor;
}

static int
PlotSamples(FBGraphRenderState const* state)
{
  return 1000;
}

void
FFEffectRenderGraph(FBModuleGraphComponentData* graphData)
{
  EffectGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false;
  renderData.graphData = graphData;
  renderData.plotSamplesSelector = PlotSamples;
  renderData.staticModuleIndex = (int)FFModuleType::Effect;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].effect[slot]; };
  renderData.voiceAudioOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].effect[slot].output; };
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  FBRenderModuleGraph<false, true>(renderData, 0);
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}