#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>

#include <vector>
#include <functional>

struct FBStaticTopo;
struct FBModuleGraphComponentData;

struct FBModuleGraphPlotParams
{
  int sampleCount = {};
  float sampleRate = {};
  bool autoSampleRate = {};
  int staticModuleIndex = -1;
};

typedef std::function<FBModuleGraphPlotParams(
  FBModuleGraphComponentData const*, int graphIndex)>
FBModuleGraphPlotParamsSelector;
typedef std::function<FBModuleProcExchangeStateBase const* (
  void const* exchangeState, int moduleSlot, int graphIndex)>
FBModuleGraphGlobalExchangeSelector;    
typedef std::function<FBModuleProcExchangeStateBase const* (
  void const* exchangeState, int voice, int moduleSlot, int graphIndex)>
FBModuleGraphVoiceExchangeSelector;    

typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int moduleSlot, int graphIndex)>
FBModuleGraphGlobalMonoOutputSelector;
typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int voice, int moduleSlot, int graphIndex)>
FBModuleGraphVoiceMonoOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int voice, int moduleSlot, int graphIndex)>
FBModuleGraphVoiceStereoOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int moduleSlot, int graphIndex)>
FBModuleGraphGlobalStereoOutputSelector;

template <class Derived>
struct FBModuleGraphRenderData
{
  FBModuleGraphComponentData* graphData = {};
  FBModuleGraphPlotParamsSelector plotParamsSelector = {};
  FBModuleGraphVoiceExchangeSelector voiceExchangeSelector = {};
  FBModuleGraphGlobalExchangeSelector globalExchangeSelector = {};
  FBModuleGraphVoiceMonoOutputSelector voiceMonoOutputSelector = {};
  FBModuleGraphGlobalMonoOutputSelector globalMonoOutputSelector = {};
  FBModuleGraphVoiceStereoOutputSelector voiceStereoOutputSelector = {};
  FBModuleGraphGlobalStereoOutputSelector globalStereoOutputSelector = {};

  int Process(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice) 
  { return static_cast<Derived*>(this)->DoProcess(state, graphIndex, exchange, exchangeVoice); }
  void BeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice) 
  { static_cast<Derived*>(this)->DoBeginVoiceOrBlock(state, graphIndex, exchange, exchangeVoice); }
  void ReleaseOnDemandBuffers(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
  { static_cast<Derived*>(this)->DoReleaseOnDemandBuffers(state, graphIndex, exchange, exchangeVoice); }
  void ProcessIndicators(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
  { return static_cast<Derived*>(this)->DoProcessIndicators(state, graphIndex, exchange, exchangeVoice, points); }
  void PostProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
  { return static_cast<Derived*>(this)->DoPostProcess(state, graphIndex, exchange, exchangeVoice, points); }
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
  moduleProcState->input->noteEvents->clear();

  renderData.BeginVoiceOrBlock(renderState, graphIndex, exchange, exchangeVoice);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.Process(renderState, graphIndex, exchange, exchangeVoice);
    if constexpr (Global)
    {
      if constexpr(Stereo)
        renderData.globalStereoOutputSelector(
          moduleProcState->procRaw,
          moduleSlot, graphIndex)->CopyTo(seriesStereoIn);
      else
        renderData.globalMonoOutputSelector(
          moduleProcState->procRaw,
          moduleSlot, graphIndex)->CopyTo(seriesMonoIn);
    }
    else
    {
      if constexpr (Stereo)
        renderData.voiceStereoOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot, graphIndex)->CopyTo(seriesStereoIn);
      else
        renderData.voiceMonoOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot, graphIndex)->CopyTo(seriesMonoIn);
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

  renderData.ProcessIndicators(renderState, graphIndex, exchange, exchangeVoice, seriesOut);
  renderData.PostProcess(renderState, graphIndex, exchange, exchangeVoice, seriesOut);
  renderData.ReleaseOnDemandBuffers(renderState, graphIndex, exchange, exchangeVoice);
}

template <bool Global, bool Stereo, class Derived>
void
FBRenderModuleGraph(FBModuleGraphRenderData<Derived>& renderData, int graphIndex) 
{
  auto graphData = renderData.graphData;
  auto renderState = graphData->renderState;
  auto guiRenderType = graphData->guiRenderType;
  auto moduleProcState = renderState->ModuleProcState();
  auto exchangeState = renderState->ExchangeContainer()->Raw();

  FB_ASSERT(renderData.graphData != nullptr);
  FB_ASSERT(renderData.plotParamsSelector != nullptr);

  if constexpr (Global)
  {
    FB_ASSERT(renderData.globalExchangeSelector != nullptr);
    if constexpr(Stereo)
      FB_ASSERT(renderData.globalStereoOutputSelector != nullptr);
    else
      FB_ASSERT(renderData.globalMonoOutputSelector != nullptr);
  } else {
    FB_ASSERT(renderData.voiceExchangeSelector != nullptr);
    if constexpr (Stereo)
      FB_ASSERT(renderData.voiceStereoOutputSelector != nullptr);
    else
      FB_ASSERT(renderData.voiceMonoOutputSelector != nullptr);
  }

  moduleProcState->anyExchangeActive = false;
  auto plotParams = renderData.plotParamsSelector(graphData, graphIndex);
  int maxDspSampleCount = plotParams.sampleCount;
  if (maxDspSampleCount == 0)
    return;

  if (guiRenderType == FBGUIRenderType::Full)
  {
    if constexpr (Global)
    {
      auto moduleExchange = renderData.globalExchangeSelector(
        exchangeState, moduleProcState->moduleSlot, graphIndex);
      moduleProcState->anyExchangeActive |= moduleExchange->active;
      if (moduleExchange->active)
        maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->LengthSamples(graphIndex));
    }
    else for (int v = 0; v < FBMaxVoices; v++)
    {
      auto moduleExchange = renderData.voiceExchangeSelector(
        exchangeState, v, moduleProcState->moduleSlot, graphIndex);
      moduleProcState->anyExchangeActive |= moduleExchange->active;
      if (moduleExchange->active)
        maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->LengthSamples(graphIndex));
    }
  }

  float guiSampleRate;
  float guiSampleCount;
  auto hostExchange = renderState->ExchangeContainer()->Host();
  if (plotParams.autoSampleRate)
  {
    guiSampleCount = static_cast<float>(graphData->pixelWidth);
    guiSampleRate = hostExchange->sampleRate / (maxDspSampleCount / guiSampleCount);
  }
  else
  {
    guiSampleRate = plotParams.sampleRate;
    guiSampleCount = static_cast<float>(plotParams.sampleCount);
  }
  renderState->PrepareForRenderPrimary(guiSampleRate, hostExchange->bpm);
  if constexpr(!Global)
    renderState->PrepareForRenderPrimaryVoice();
  moduleProcState->renderType = FBRenderType::GraphPrimary;
  FBRenderModuleGraphSeries<Global, Stereo>(renderData, false, -1, graphIndex, graphData->graphs[graphIndex].primarySeries);
  
  if (guiRenderType == FBGUIRenderType::Basic)
    return;

  renderState->PrepareForRenderExchange(hostExchange->noteMatrix);
  if constexpr (Global)
  {
    auto moduleExchange = renderData.globalExchangeSelector(
      exchangeState, moduleProcState->moduleSlot, graphIndex);
    if (!moduleExchange->ShouldGraph(graphIndex))
      return;
    float positionNormalized = moduleExchange->PositionNormalized(graphIndex);
    if (graphData->skipDrawOnEqualsPrimary &&
      renderState->GlobalModuleExchangeStateEqualsPrimary(
      plotParams.staticModuleIndex, moduleProcState->moduleSlot))
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
      exchangeState, v, moduleProcState->moduleSlot, graphIndex);
    if (!moduleExchange->ShouldGraph(graphIndex))
      continue;
    renderState->PrepareForRenderExchangeVoice(v);
    float positionNormalized = moduleExchange->PositionNormalized(graphIndex);
    if (graphData->skipDrawOnEqualsPrimary &&
      renderState->VoiceModuleExchangeStateEqualsPrimary(
      v, plotParams.staticModuleIndex, moduleProcState->moduleSlot))
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