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
  int thisPlotSamples = {};
  int totalPlotSamples = {};
  int DoProcess(FBModuleProcState& state);
  void DoBeginVoice(FBModuleProcState& state);
  FFEffectProcessor& GetProcessor(FBModuleProcState& state);
  void DoProcessIndicators(bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

void 
EffectGraphRenderData::DoBeginVoice(FBModuleProcState& state) 
{ 
  thisPlotSamples = 0;
  GetProcessor(state).BeginVoice(state); 
}

int 
EffectGraphRenderData::DoProcess(FBModuleProcState& state) 
{ 
  GetProcessor(state).Process(state); 
  thisPlotSamples -= FBFixedBlockSamples;
  return std::max(0, thisPlotSamples);
}

FFEffectProcessor&
EffectGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].effect[state.moduleSlot].processor;
}

static int
PlotSamples(FBModuleGraphComponentData const* data)
{
  return data->pixelWidth;
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
  renderData.totalPlotSamples = PlotSamples(graphData);
  renderData.staticModuleIndex = (int)FFModuleType::Effect;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].effect[slot]; };
  renderData.voiceAudioOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].effect[slot].output; };
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  FBRenderModuleGraph<false, true>(renderData, 0);
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}