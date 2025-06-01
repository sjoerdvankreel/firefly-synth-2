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
  int totalSamples = {};
  std::array<int, FFEffectBlockCount + 1> samplesProcessed = {};

  FFEffectProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoice(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

static int
PlotSamples(FBModuleGraphComponentData const* data)
{
  // Need exactly as many samples for the dsp side as the gui side.
  // This way we guarantee rendering for the graph is done with audio sample rate.
  // This is important for the filters.
  return data->pixelWidth;
}

FFEffectProcessor&
EffectGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].effect[state.moduleSlot].processor;
}

void 
EffectGraphRenderData::DoBeginVoice(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).BeginVoice(true, graphIndex, totalSamples, *moduleProcState);
}

int 
EffectGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  bool plotSpecificFilter = false;
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::On, 0 };
  bool on = state->AudioParamBool(indices, false, -1);
  if (!on)
    return 0;

  if (graphIndex != FFEffectBlockCount)
  {
    indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Kind, graphIndex };
    auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);
    plotSpecificFilter = kind == FFEffectKind::StVar || kind == FFEffectKind::Comb;
    if (kind == FFEffectKind::Off)
      return 0;
  }

  auto* procState = moduleProcState->ProcAs<FFProcState>();
  auto& input = procState->dsp.voice[moduleProcState->voice->slot].effect[moduleProcState->moduleSlot].input;
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      if (plotSpecificFilter)
        input[c].Set(s, samplesProcessed[graphIndex] == 0 ? 1.0f : 0.0f);
      else
        input[c].Set(s, ((samplesProcessed[graphIndex] + s) / static_cast<float>(totalSamples)) * 2.0f - 1.0f);
  
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).Process(*moduleProcState);
}

void
FFEffectRenderGraph(FBModuleGraphComponentData* graphData)
{
  EffectGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  renderData.graphData = graphData;
  renderData.plotSamplesSelector = PlotSamples;
  renderData.totalSamples = PlotSamples(graphData);
  renderData.staticModuleIndex = (int)FFModuleType::Effect;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].effect[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].effect[slot].output[0]; };

  auto* renderState = graphData->renderState;
  auto* moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::On, 0 };
  bool on = renderState->AudioParamBool(indices, false, -1);
  for (int i = 0; i <= FFEffectBlockCount; i++)
  {
    FBRenderModuleGraph<false, false>(renderData, i);
    if (i == FFEffectBlockCount)
      graphData->graphs[i].text = on? "ALL": "ALL OFF";
    else
    {
      FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Kind, i };
      auto kind = renderState->AudioParamList<FFEffectKind>(indices, false, -1);
      bool blockOn = on && kind != FFEffectKind::Off;
      graphData->graphs[i].text = std::to_string(i + 1);
      if (!blockOn)
        graphData->graphs[i].text += " OFF";
    }
  }
}