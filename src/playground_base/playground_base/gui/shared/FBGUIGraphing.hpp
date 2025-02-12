#pragma once

#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <vector>
#include <cassert>
#include <functional>

class FBFixedFloatBlock;
struct FBStaticTopo;
struct FBModuleGraphComponentData;

typedef std::function<FBFixedFloatBlock const* (
  void const* procState, int moduleSlot)>
FBModuleGraphGlobalOutputSelector;
typedef std::function<FBFixedFloatBlock const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceOutputSelector;

typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int moduleSlot)>
FBModuleGraphGlobalExchangeSelector;    
typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int voice, int moduleSlot)>
FBModuleGraphVoiceExchangeSelector;    

typedef std::function<int(
  FBStaticTopo const& topo, void const* scalarState, int moduleSlot, float sampleRate, float bpm)>
FBModuleGraphPlotLengthSelector;

template <class Processor>
struct FBModuleGraphRenderData final
{
  Processor processor = {};
  int staticModuleIndex = -1;
  FBModuleGraphComponentData* graphData = {};
  FBModuleGraphPlotLengthSelector plotLengthSelector = {};
  FBModuleGraphVoiceOutputSelector voiceOutputSelector = {};
  FBModuleGraphGlobalOutputSelector globalOutputSelector = {};
  FBModuleGraphVoiceExchangeSelector voiceExchangeSelector = {};
  FBModuleGraphGlobalExchangeSelector globalExchangeSelector = {};
};

template <bool Global, class Processor> 
void 
FBRenderModuleGraphSeries(
  FBModuleGraphRenderData<Processor>& renderData, 
  std::vector<float>& seriesOut)
{
  seriesOut.clear();
  FBFixedFloatArray seriesIn;
  int processed = FBFixedBlockSamples;
  auto renderState = renderData.graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;

  if constexpr(Global)
    renderData.processor.Reset(*moduleProcState);
  else
    renderData.processor.BeginVoice(*moduleProcState);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.processor.Process(*moduleProcState);
    if constexpr(Global)
      renderData.globalOutputSelector(
        moduleProcState->procRaw,
        moduleSlot)->StoreToFloatArray(seriesIn);
    else
      renderData.voiceOutputSelector(
        moduleProcState->procRaw, 
        moduleProcState->voice->slot, 
        moduleSlot)->StoreToFloatArray(seriesIn);
    for (int i = 0; i < processed; i++)
      seriesOut.push_back(seriesIn.data[i]);
  }
}

template <bool Global, class RenderData>
void
FBRenderModuleGraph(RenderData& renderData)
{
  auto graphData = renderData.graphData;
  auto renderState = graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  auto scalarState = renderState->ScalarContainer()->Raw();
  auto exchangeState = renderState->ExchangeContainer()->Raw();

  assert(renderData.graphData != nullptr);
  assert(renderData.staticModuleIndex != -1);
  assert(renderData.plotLengthSelector != nullptr);
  assert((renderData.voiceOutputSelector == nullptr) != (renderData.globalOutputSelector == nullptr));
  assert((renderData.voiceExchangeSelector == nullptr) != (renderData.globalExchangeSelector == nullptr));

  graphData->text = "OFF";
  float dspBpm = renderState->ExchangeContainer()->Bpm();
  float dspSampleRate = renderState->ExchangeContainer()->SampleRate();
  int maxDspSampleCount = renderData.plotLengthSelector(
    moduleProcState->topo->static_, scalarState, moduleProcState->moduleSlot, dspSampleRate, dspBpm);

  if constexpr (Global)
  {
    auto moduleExchange = renderData.globalExchangeSelector(
      exchangeState, moduleProcState->moduleSlot);
    if (moduleExchange->active)
      maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->lengthSamples);
  }
  else for (int v = 0; v < FBMaxVoices; v++)
  {
    auto moduleExchange = renderData.voiceExchangeSelector(
      exchangeState, v, moduleProcState->moduleSlot);
    if(moduleExchange->active)
      maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->lengthSamples);
  }

  float guiSampleCount = (float)graphData->pixelWidth;
  moduleProcState->sampleRate = dspSampleRate / (maxDspSampleCount / guiSampleCount);
  renderState->PrepareForRenderPrimary();
  if constexpr(!Global)
    renderState->PrepareForRenderPrimaryVoice();
  FBRenderModuleGraphSeries<Global>(renderData, graphData->primarySeries);
  float guiDurationSeconds = renderData.graphData->primarySeries.size() / moduleProcState->sampleRate;
  renderData.graphData->text = FBFormatFloat(guiDurationSeconds, FBDefaultDisplayPrecision) + " Sec";
  
  renderState->PrepareForRenderExchange();
  if constexpr (Global)
  {
    auto moduleExchange = renderData.globalExchangeSelector(
      exchangeState, moduleProcState->moduleSlot);
    if (!moduleExchange->ShouldGraph())
      return;
    float positionNormalized = moduleExchange->PositionNormalized();
    if (renderState->GlobalModuleExchangeStateEqualsPrimary(
      renderData.staticModuleIndex, moduleProcState->moduleSlot))
    {
      graphData->primaryMarkers.push_back(
        (int)(positionNormalized * graphData->primarySeries.size()));
      return;
    }
    auto& secondary = graphData->secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<Global>(renderData, secondary.points);
    secondary.marker = (int)(positionNormalized * secondary.points.size());
  } else for (int v = 0; v < FBMaxVoices; v++)
  {
    auto moduleExchange = renderData.voiceExchangeSelector(
      exchangeState, v, moduleProcState->moduleSlot);
    if (!moduleExchange->ShouldGraph())
      continue;
    renderState->PrepareForRenderExchangeVoice(v);
    float positionNormalized = moduleExchange->PositionNormalized();
    if (renderState->VoiceModuleExchangeStateEqualsPrimary(
      v, renderData.staticModuleIndex, moduleProcState->moduleSlot))
    {
      graphData->primaryMarkers.push_back(
        (int)(positionNormalized * graphData->primarySeries.size()));
      continue;
    }
    auto& secondary = graphData->secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<false>(renderData, secondary.points);
    secondary.marker = (int)(positionNormalized * secondary.points.size());
  }
}