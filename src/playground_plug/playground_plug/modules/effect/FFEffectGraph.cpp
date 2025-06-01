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
  std::array<int, FFEffectBlockCount> samplesProcessed = {};

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
  GetProcessor(*moduleProcState).BeginVoice(graphIndex, *moduleProcState);
}

int 
EffectGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto* procState = moduleProcState->ProcAs<FFProcState>(); 
  auto& input = procState->dsp.voice[moduleProcState->voice->slot].effect[moduleProcState->moduleSlot].input;
  FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Kind, graphIndex };
  auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      if (kind == FFEffectKind::StVar || kind == FFEffectKind::Comb)
        input[c].Set(s, samplesProcessed[graphIndex] == 0 ? 1.0f : 0.0f);
      else
        input[c].Set(s, ((samplesProcessed[graphIndex] + s) / static_cast<float>(totalSamples)) * 2.0f - 1.0f);
  GetProcessor(*moduleProcState).Process(*moduleProcState);
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return std::clamp(totalSamples - samplesProcessed[graphIndex], 0, FBFixedBlockSamples);
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
  renderData.totalSamples = PlotSamples(graphData);
  renderData.staticModuleIndex = (int)FFModuleType::Effect;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].effect[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].effect[slot].output[0]; };
  for(int i = 0; i < FFEffectBlockCount; i++)
    FBRenderModuleGraph<false, false>(renderData, i);
}