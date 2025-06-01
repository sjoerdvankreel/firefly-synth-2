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
  FBModuleGraphComponentData const*)>
FBModuleGraphPlotSamplesSelector;
typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int moduleSlot)>
FBModuleGraphGlobalExchangeSelector;    
typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int voice, int moduleSlot)>
FBModuleGraphVoiceExchangeSelector;    

typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int moduleSlot)>
FBModuleGraphGlobalMonoOutputSelector;
typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceMonoOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceStereoOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int moduleSlot)>
FBModuleGraphGlobalStereoOutputSelector;

template <class Derived>
struct FBModuleGraphRenderData
{
  int staticModuleIndex = -1;
  FBModuleGraphComponentData* graphData = {};
  FBModuleGraphPlotSamplesSelector plotSamplesSelector = {};
  FBModuleGraphVoiceExchangeSelector voiceExchangeSelector = {};
  FBModuleGraphGlobalExchangeSelector globalExchangeSelector = {};
  FBModuleGraphVoiceMonoOutputSelector voiceMonoOutputSelector = {};
  FBModuleGraphGlobalMonoOutputSelector globalMonoOutputSelector = {};
  FBModuleGraphVoiceStereoOutputSelector voiceStereoOutputSelector = {};
  FBModuleGraphGlobalStereoOutputSelector globalStereoOutputSelector = {};

  void Reset(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice) 
  { static_cast<Derived*>(this)->DoReset(state, graphIndex, exchange, exchangeVoice); }
  int Process(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice) 
  { return static_cast<Derived*>(this)->DoProcess(state, graphIndex, exchange, exchangeVoice); }
  void BeginVoice(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice) 
  { static_cast<Derived*>(this)->DoBeginVoice(state, graphIndex, exchange, exchangeVoice); }
  void ProcessIndicators(int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
  { return static_cast<Derived*>(this)->DoProcessIndicators(graphIndex, exchange, exchangeVoice, points); }
};

template <bool Global, bool Stereo, class Derived> 
void 
FBRenderModuleGraphSeries(
  FBModuleGraphRenderData<Derived>& renderData,
  bool exchange, int exchangeVoice, int graphIndex,
  FBModuleGraphPoints& seriesOut)
{
  seriesOut.l.clear();
  seriesOut.r.clear();
  seriesOut.verticalIndicators.clear();
  
  FBSArray<float, FBFixedBlockSamples> seriesMonoIn = {};
  FBSArray2<float, FBFixedBlockSamples, 2> seriesStereoIn = {};

  int processed = FBFixedBlockSamples;
  auto renderState = renderData.graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  moduleProcState->input->note->clear();

  if constexpr (Global)
    renderData.Reset(renderState, graphIndex, exchange, exchangeVoice);
  else
    renderData.BeginVoice(renderState, graphIndex, exchange, exchangeVoice);

  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.Process(renderState, graphIndex, exchange, exchangeVoice);
    if constexpr (Global)
    {
      if constexpr(Stereo)
        renderData.globalStereoOutputSelector(
          moduleProcState->procRaw,
          moduleSlot)->CopyTo(seriesStereoIn);
      else
        renderData.globalMonoOutputSelector(
          moduleProcState->procRaw,
          moduleSlot)->CopyTo(seriesMonoIn);
    }
    else
    {
      if constexpr (Stereo)
        renderData.voiceStereoOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot)->CopyTo(seriesStereoIn);
      else
        renderData.voiceMonoOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot)->CopyTo(seriesMonoIn);
    }
    if constexpr (Stereo)
    {
      seriesStereoIn.NaNCheck();
      for (int i = 0; i < processed; i++)
      {
        seriesOut.l.push_back(seriesStereoIn[0].Get(i));
        seriesOut.r.push_back(seriesStereoIn[1].Get(i));
      }
    }
    else
    {
      seriesMonoIn.NaNCheck();
      for (int i = 0; i < processed; i++)
        seriesOut.l.push_back(seriesMonoIn.Get(i));
    }
  }

  renderData.ProcessIndicators(graphIndex, exchange, exchangeVoice, seriesOut);
}

template <bool Global, bool Stereo, class Derived>
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
    if constexpr(Stereo)
      assert(renderData.globalStereoOutputSelector != nullptr);
    else
      assert(renderData.globalMonoOutputSelector != nullptr);
  } else {
    assert(renderData.voiceExchangeSelector != nullptr);
    if constexpr (Stereo)
      assert(renderData.voiceStereoOutputSelector != nullptr);
    else
      assert(renderData.voiceMonoOutputSelector != nullptr);
  }

  graphData->graphs[graphIndex].text = "OFF";
  moduleProcState->anyExchangeActive = false;
  int maxDspSampleCount = renderData.plotSamplesSelector(graphData);

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
  FBRenderModuleGraphSeries<Global, Stereo>(renderData, false, -1, graphIndex, graphData->graphs[graphIndex].primarySeries);
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
    FBRenderModuleGraphSeries<Global, Stereo>(renderData, true, -1, graphIndex, secondary.points);
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
    FBRenderModuleGraphSeries<false, Stereo>(renderData, true, v, graphIndex, secondary.points);
    secondary.marker = static_cast<int>(positionNormalized * secondary.points.l.size());
  }
}