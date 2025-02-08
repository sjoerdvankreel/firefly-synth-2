#pragma once

#include <playground_base/base/shared/FBFormat.hpp>

#include <vector>
#include <cassert>
#include <functional>

class FBFixedFloatBlock;
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

template <class Processor>
struct FBModuleGraphRenderData final
{
  int moduleType = -1;
  Processor processor = {};
  FBModuleGraphComponentData* graphData = {};
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
  assert(renderData.moduleType != -1);
  assert(renderData.graphData != nullptr);
  assert((renderData.voiceOutputSelector == nullptr) != (renderData.globalOutputSelector == nullptr));
  assert((renderData.voiceExchangeSelector == nullptr) != (renderData.globalExchangeSelector == nullptr));

  auto graphData = renderData.graphData;
  auto renderState = graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  auto scalarState = renderState->ScalarContainer()->Raw();
  auto exchangeState = renderState->ExchangeContainer()->Raw();

  // TODO voice
  FBModuleProcExchangeState const* moduleExchange = nullptr;
  if constexpr (Global)
    moduleExchange = renderData.globalExchangeSelector(exchangeState, moduleProcState->moduleSlot);

  // TODO per voice
  graphData->text = "OFF";
  if (!moduleExchange->active)
    return;

  float guiSampleCount = (float)graphData->pixelWidth;
  float dspSampleCount = (float)moduleExchange->lengthSamples;
  float dspSampleRate = renderState->ExchangeContainer()->SampleRate();
  moduleProcState->sampleRate = dspSampleRate / (dspSampleCount / guiSampleCount);  

  renderState->PrepareForRenderPrimary();
  FBRenderModuleGraphSeries<Global>(renderData, graphData->primarySeries);
  float durationSeconds = renderData.graphData->primarySeries.size() / moduleProcState->sampleRate;
  renderData.graphData->text = FBFormatFloat(durationSeconds, FBDefaultDisplayPrecision) + " Sec";
  
  renderState->PrepareForRenderExchange();  
  assert(moduleExchange->positionSamples < moduleExchange->lengthSamples);
  float positionNormalized = moduleExchange->positionSamples / (float)moduleExchange->lengthSamples;

  // TODO voice
  if (renderState->GlobalModuleExchangeStateEqualsPrimary((int)FFModuleType::GLFO, moduleProcState->moduleSlot))
  {
    graphData->primaryMarkers.push_back((int)(positionNormalized * graphData->primarySeries.size()));
    assert(graphData->primaryMarkers[graphData->primaryMarkers.size() - 1] < graphData->primarySeries.size());
    return;
  }

  auto& secondary = graphData->secondarySeries.emplace_back();
  FBRenderModuleGraphSeries<Global>(renderData, secondary.points);
  secondary.marker = (int)(positionNormalized * secondary.points.size());
  assert(secondary.marker < secondary.points.size());
}