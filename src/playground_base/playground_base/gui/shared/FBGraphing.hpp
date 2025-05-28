#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <vector>
#include <cassert>
#include <functional>

struct FBStaticTopo;
struct FBModuleGraphComponentData;

typedef std::function<int(
  FBGraphRenderState const*)>
FBModuleGraphPlotSamplesSelector;
typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int moduleSlot)>
FBModuleGraphGlobalExchangeSelector;    
typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int voice, int moduleSlot)>
FBModuleGraphVoiceExchangeSelector;    

typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int moduleSlot)>
FBModuleGraphGlobalCVOutputSelector;
typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceCVOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceAudioOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int moduleSlot)>
FBModuleGraphGlobalAudioOutputSelector;

template <class Derived>
struct FBModuleGraphRenderData
{
  int staticModuleIndex = -1;
  FBModuleGraphComponentData* graphData = {};
  FBModuleGraphPlotSamplesSelector plotSamplesSelector = {};
  FBModuleGraphVoiceExchangeSelector voiceExchangeSelector = {};
  FBModuleGraphGlobalExchangeSelector globalExchangeSelector = {};
  FBModuleGraphVoiceCVOutputSelector voiceCVOutputSelector = {};
  FBModuleGraphGlobalCVOutputSelector globalCVOutputSelector = {};
  FBModuleGraphVoiceAudioOutputSelector voiceAudioOutputSelector = {};
  FBModuleGraphGlobalAudioOutputSelector globalAudioOutputSelector = {};

  void Reset(FBModuleProcState& state) { static_cast<Derived*>(this)->DoReset(state); }
  int Process(FBModuleProcState& state) { return static_cast<Derived*>(this)->DoProcess(state); }
  void BeginVoice(FBModuleProcState& state) { static_cast<Derived*>(this)->DoBeginVoice(state); }
  void ProcessIndicators(bool exchange, int exchangeVoice, FBModuleGraphPoints& points) 
  { return static_cast<Derived*>(this)->DoProcessIndicators(exchange, exchangeVoice, points); }
};

template <bool Global, bool Audio, class Derived> 
void 
FBRenderModuleGraphSeries(
  FBModuleGraphRenderData<Derived>& renderData,
  bool exchange, int exchangeVoice, 
  FBModuleGraphPoints& seriesOut)
{
  seriesOut.l.clear();
  seriesOut.r.clear();
  seriesOut.verticalIndicators.clear();
  
  FBSArray<float, FBFixedBlockSamples> seriesCVIn = {};
  FBSArray2<float, FBFixedBlockSamples, 2> seriesAudioIn = {};

  int processed = FBFixedBlockSamples;
  auto renderState = renderData.graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  moduleProcState->input->note->clear();

  if constexpr (Global)
    renderData.Reset(*moduleProcState);
  else
    renderData.BeginVoice(*moduleProcState);

  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.Process(*moduleProcState);
    if constexpr (Global)
    {
      if constexpr(Audio)
        renderData.globalAudioOutputSelector(
          moduleProcState->procRaw,
          moduleSlot)->CopyTo(seriesAudioIn);
      else
        renderData.globalCVOutputSelector(
          moduleProcState->procRaw,
          moduleSlot)->CopyTo(seriesCVIn);
    }
    else
    {
      if constexpr (Audio)
        renderData.voiceAudioOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot)->CopyTo(seriesAudioIn);
      else
        renderData.voiceCVOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot)->CopyTo(seriesCVIn);
    }
    if constexpr (Audio)
    {
      seriesAudioIn.NaNCheck();
      for (int i = 0; i < processed; i++)
      {
        seriesOut.l.push_back(seriesAudioIn[0].Get(i));
        seriesOut.r.push_back(seriesAudioIn[1].Get(i));
      }
    }
    else
    {
      seriesCVIn.NaNCheck();
      for (int i = 0; i < processed; i++)
        seriesOut.l.push_back(seriesCVIn.Get(i));
    }
  }

  renderData.ProcessIndicators(exchange, exchangeVoice, seriesOut);
}

template <bool Global, bool Audio, class Derived>
void
FBRenderModuleGraph(FBModuleGraphRenderData<Derived>& renderData, int graphIndex)
{
  auto graphData = renderData.graphData;
  auto renderState = graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  auto exchangeState = renderState->ExchangeContainer()->Raw();

  assert(renderData.graphData != nullptr);
  assert(renderData.staticModuleIndex != -1);
  assert(renderData.plotSamplesSelector != nullptr);

  if constexpr (Global)
  {
    assert(renderData.globalExchangeSelector != nullptr);
    if constexpr(Audio)
      assert(renderData.globalAudioOutputSelector != nullptr);
    else
      assert(renderData.globalCVOutputSelector != nullptr);
  } else {
    assert(renderData.voiceExchangeSelector != nullptr);
    if constexpr (Audio)
      assert(renderData.voiceAudioOutputSelector != nullptr);
    else
      assert(renderData.voiceCVOutputSelector != nullptr);
  }

  graphData->graphs[graphIndex].text = "OFF";
  moduleProcState->anyExchangeActive = false;
  int maxDspSampleCount = renderData.plotSamplesSelector(renderState);

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

  auto hostExchange = renderState->ExchangeContainer()->Host();
  float guiSampleCount = static_cast<float>(graphData->pixelWidth);
  float dspSampleRate = renderState->ExchangeContainer()->Host()->sampleRate;
  float guiSampleRate = hostExchange->sampleRate / (maxDspSampleCount / guiSampleCount);
  renderState->PrepareForRenderPrimary(guiSampleRate, hostExchange->bpm);
  if constexpr(!Global)
    renderState->PrepareForRenderPrimaryVoice();
  moduleProcState->renderType = FBRenderType::GraphPrimary;
  FBRenderModuleGraphSeries<Global, Audio>(renderData, false, -1, graphData->graphs[graphIndex].primarySeries);
  float guiDurationSeconds = renderData.graphData->graphs[graphIndex].primarySeries.l.size() / moduleProcState->input->sampleRate;
  if(guiDurationSeconds > 0.0f)
    renderData.graphData->graphs[graphIndex].text = FBFormatDouble(guiDurationSeconds, FBDefaultDisplayPrecision) + " Sec";
  
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
      graphData->graphs[graphIndex].primaryMarkers.push_back(
        static_cast<int>(positionNormalized * graphData->graphs[graphIndex].primarySeries.l.size()));
      return;
    }
    moduleProcState->renderType = FBRenderType::GraphExchange;
    auto& secondary = graphData->graphs[graphIndex].secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<Global, Audio>(renderData, true, -1, secondary.points);
    secondary.marker = static_cast<int>(positionNormalized * secondary.points.l.size());
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
      graphData->graphs[graphIndex].primaryMarkers.push_back(
        static_cast<int>(positionNormalized * graphData->graphs[graphIndex].primarySeries.l.size()));
      continue;
    }
    moduleProcState->renderType = FBRenderType::GraphExchange;
    auto& secondary = graphData->graphs[graphIndex].secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<false, Audio>(renderData, true, v, secondary.points);
    secondary.marker = static_cast<int>(positionNormalized * secondary.points.l.size());
  }
}