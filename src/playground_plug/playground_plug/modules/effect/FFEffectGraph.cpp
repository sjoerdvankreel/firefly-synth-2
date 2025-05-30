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
  int DoProcess(FBModuleProcState& state, int graphIndex);
  void DoBeginVoice(FBModuleProcState& state, int graphIndex);
  void DoProcessIndicators(bool exchange, int exchangeVoice, int graphIndex, FBModuleGraphPoints& points) {}
};

void 
EffectGraphRenderData::DoBeginVoice(FBModuleProcState& state, int graphIndex)
{ 
  samplesProcessed[graphIndex] = 0;
  GetProcessor(state).BeginVoice(graphIndex, state); 
}

int 
EffectGraphRenderData::DoProcess(FBModuleProcState& state, int graphIndex)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& input = procState->dsp.voice[state.voice->slot].effect[state.moduleSlot].input;
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      input[c].Set(s, ((samplesProcessed[graphIndex] + s) / static_cast<float>(totalSamples)) * 2.0f - 1.0f);
  GetProcessor(state).Process(state); 
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return std::clamp(totalSamples - samplesProcessed[graphIndex], 0, FBFixedBlockSamples);
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
  renderData.totalSamples = PlotSamples(graphData);
  renderData.staticModuleIndex = (int)FFModuleType::Effect;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].effect[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].effect[slot].output[0]; };
  for(int i = 0; i < FFEffectBlockCount; i++)
    FBRenderModuleGraph<false, false>(renderData, i);
}