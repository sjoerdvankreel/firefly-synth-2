#pragma once

#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <vector>
#include <cassert>
#include <functional>

class FBFixedFloatBlock;
struct FBStaticTopo;
struct FBModuleGraphComponentData;

FBNoteEvent
FBDetailMakeNoteC4Off(int pos);

struct FBModuleGraphPlotParams final
{
  int samples = -1;
  int releaseAt = -1;
};

typedef std::function<FBModuleGraphPlotParams(
  FBGraphRenderState const*)>
FBModuleGraphPlotParamsSelector;

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

template <class Processor>
struct FBModuleGraphRenderData final
{
  Processor processor = {};
  int staticModuleIndex = -1;
  FBModuleGraphComponentData* graphData = {};
  FBModuleGraphPlotParamsSelector plotParamsSelector = {};
  FBModuleGraphVoiceOutputSelector voiceOutputSelector = {};
  FBModuleGraphGlobalOutputSelector globalOutputSelector = {};
  FBModuleGraphVoiceExchangeSelector voiceExchangeSelector = {};
  FBModuleGraphGlobalExchangeSelector globalExchangeSelector = {};
};

template <bool Global, class Processor> 
void 
FBRenderModuleGraphSeries(
  FBModuleGraphRenderData<Processor>& renderData, 
  int releaseAt, std::vector<float>& seriesOut)
{
  seriesOut.clear();
  FBFixedFloatArray seriesIn;
  bool released = false;
  int processed = FBFixedBlockSamples;
  bool shouldRelease = releaseAt >= 0;
  auto renderState = renderData.graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  moduleProcState->input->note->clear();

  if constexpr(Global)
    renderData.processor.Reset(*moduleProcState);
  else
    renderData.processor.BeginVoice(*moduleProcState);
  while (processed == FBFixedBlockSamples)
  {
    if (shouldRelease && !released && releaseAt < FBFixedBlockSamples)
    {
      released = true;
      moduleProcState->input->note->push_back(FBDetailMakeNoteC4Off(releaseAt));
    }
    processed = renderData.processor.Process(*moduleProcState);
    if (shouldRelease && !released)
      releaseAt -= processed;
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
  auto exchangeState = renderState->ExchangeContainer()->Raw();

  assert(renderData.graphData != nullptr);
  assert(renderData.staticModuleIndex != -1);
  assert(renderData.plotParamsSelector != nullptr);
  assert((renderData.voiceOutputSelector == nullptr) != (renderData.globalOutputSelector == nullptr));
  assert((renderData.voiceExchangeSelector == nullptr) != (renderData.globalExchangeSelector == nullptr));

  graphData->text = "OFF";
  moduleProcState->anyExchangeActive = false;
  auto plotParams = renderData.plotParamsSelector(renderState);
  int maxDspSampleCount = plotParams.samples;

  if constexpr (Global)
  {
    auto moduleExchange = renderData.globalExchangeSelector(
      exchangeState, moduleProcState->moduleSlot);
    moduleProcState->anyExchangeActive |= moduleExchange->active;
    if (moduleExchange->active)
      maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->lengthSamples);
  }
  else for (int v = 0; v < FBMaxVoices; v++)
  {
    auto moduleExchange = renderData.voiceExchangeSelector(
      exchangeState, v, moduleProcState->moduleSlot);
    moduleProcState->anyExchangeActive |= moduleExchange->active;
    if(moduleExchange->active)
      maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->lengthSamples);
  }

  int guiReleaseAt = -1;
  auto procExchange = renderState->ExchangeContainer()->Proc();
  float guiSampleCount = static_cast<float>(graphData->pixelWidth;
  float dspSampleRate = renderState->ExchangeContainer()->Proc()->sampleRate;
  float guiSampleRate = procExchange->sampleRate / (maxDspSampleCount / guiSampleCount);
  if(plotParams.releaseAt != -1)
    guiReleaseAt = (int)std::round(guiSampleRate * plotParams.releaseAt / dspSampleRate);
  renderState->PrepareForRenderPrimary(guiSampleRate, procExchange->bpm);
  if constexpr(!Global)
    renderState->PrepareForRenderPrimaryVoice();
  moduleProcState->renderType = FBRenderType::GraphPrimary;
  FBRenderModuleGraphSeries<Global>(renderData, guiReleaseAt, graphData->primarySeries);
  float guiDurationSeconds = renderData.graphData->primarySeries.size() / moduleProcState->input->sampleRate;
  renderData.graphData->text = FBFormatDouble(guiDurationSeconds, FBDefaultDisplayPrecision) + " Sec";
  
  renderState->PrepareForRenderExchange();
  if constexpr (Global)
  {
    auto moduleExchange = renderData.globalExchangeSelector(
      exchangeState, moduleProcState->moduleSlot);
    if (!moduleExchange->ShouldGraph())
      return;
    float positionNormalized = moduleExchange->PositionNormalized();
    if (graphData->skipDrawOnEqualsPrimary &&
      renderState->GlobalModuleExchangeStateEqualsPrimary(
      renderData.staticModuleIndex, moduleProcState->moduleSlot))
    {
      graphData->primaryMarkers.push_back(
        (int)(positionNormalized * graphData->primarySeries.size()));
      return;
    }
    moduleProcState->renderType = FBRenderType::GraphExchange;
    auto& secondary = graphData->secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<Global>(renderData, -1, secondary.points);
    secondary.marker = (int)(positionNormalized * secondary.points.size());
  } else for (int v = 0; v < FBMaxVoices; v++)
  {
    auto moduleExchange = renderData.voiceExchangeSelector(
      exchangeState, v, moduleProcState->moduleSlot);
    if (!moduleExchange->ShouldGraph())
      continue;
    renderState->PrepareForRenderExchangeVoice(v);
    float positionNormalized = moduleExchange->PositionNormalized();
    if (graphData->skipDrawOnEqualsPrimary &&
      renderState->VoiceModuleExchangeStateEqualsPrimary(
      v, renderData.staticModuleIndex, moduleProcState->moduleSlot))
    {
      graphData->primaryMarkers.push_back(
        (int)(positionNormalized * graphData->primarySeries.size()));
      continue;
    }
    moduleProcState->renderType = FBRenderType::GraphExchange;
    auto& secondary = graphData->secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<false>(renderData, -1, secondary.points);
    secondary.marker = (int)(positionNormalized * secondary.points.size());
  }
}