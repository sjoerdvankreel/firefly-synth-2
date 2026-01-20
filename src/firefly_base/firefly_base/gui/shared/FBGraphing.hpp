#pragma once

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>

#include <firefly_base/base/state/main/FBGraphRenderState.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>

#include <vector>
#include <functional>

class FBGraphRenderState;
struct FBModuleGraphComponentData;

struct FBModuleGraphPlotParams
{
  int sampleCount = {};
  float sampleRate = {};
  bool autoSampleRate = {};
  int staticModuleIndex = -1;
};

typedef std::function<FBModuleGraphPlotParams(
  FBModuleGraphComponentData const*, bool detailGraphs, int graphIndex)>
FBModuleGraphPlotParamsSelector;
typedef std::function<FBModuleProcExchangeStateBase const* (
  void const* exchangeState, int moduleSlot, bool detailGraphs, int graphIndex)>
FBModuleGraphGlobalExchangeSelector;    
typedef std::function<FBModuleProcExchangeStateBase const* (
  void const* exchangeState, int voice, int moduleSlot, bool detailGraphs, int graphIndex)>
FBModuleGraphVoiceExchangeSelector;    

typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int moduleSlot, bool detailGraphs, int graphIndex)>
FBModuleGraphGlobalMonoOutputSelector;
typedef std::function<FBSArray<float, FBFixedBlockSamples> const* (
  void const* procState, int voice, int moduleSlot, bool detailGraphs, int graphIndex)>
FBModuleGraphVoiceMonoOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int voice, int moduleSlot, bool detailGraphs, int graphIndex)>
FBModuleGraphVoiceStereoOutputSelector;
typedef std::function<FBSArray2<float, FBFixedBlockSamples, 2> const* (
  void const* procState, int moduleSlot, bool detailGraphs, int graphIndex)>
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

  int Process(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice)
  { return static_cast<Derived*>(this)->DoProcess(state, detailGraphs, graphIndex, exchange, exchangeVoice); }
  void BeginVoiceOrBlock(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice)
  { static_cast<Derived*>(this)->DoBeginVoiceOrBlock(state, detailGraphs, graphIndex, exchange, exchangeVoice); }
  void ReleaseOnDemandBuffers(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice)
  { static_cast<Derived*>(this)->DoReleaseOnDemandBuffers(state, detailGraphs, graphIndex, exchange, exchangeVoice); }
  void ProcessIndicators(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
  { return static_cast<Derived*>(this)->DoProcessIndicators(state, detailGraphs, graphIndex, exchange, exchangeVoice, points); }
  void PostProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
  { return static_cast<Derived*>(this)->DoPostProcess(state, detailGraphs, graphIndex, exchange, exchangeVoice, points); }
};

template <bool Global, bool Stereo, class Derived> 
void 
FBRenderModuleGraphSeries(
  FBModuleGraphRenderData<Derived>& renderData,
  bool detailGraphs, int graphIndex,
  bool exchange, int exchangeVoice,
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

  renderData.BeginVoiceOrBlock(renderState, detailGraphs, graphIndex, exchange, exchangeVoice);
  while (processed == FBFixedBlockSamples)
  {
    processed = renderData.Process(renderState, detailGraphs, graphIndex, exchange, exchangeVoice);
    if constexpr (Global)
    {
      if constexpr(Stereo)
        renderData.globalStereoOutputSelector(
          moduleProcState->procRaw,
          moduleSlot, detailGraphs, graphIndex)->CopyTo(seriesStereoIn);
      else
        renderData.globalMonoOutputSelector(
          moduleProcState->procRaw,
          moduleSlot, detailGraphs, graphIndex)->CopyTo(seriesMonoIn);
    }
    else
    {
      if constexpr (Stereo)
        renderData.voiceStereoOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot, detailGraphs, graphIndex)->CopyTo(seriesStereoIn);
      else
        renderData.voiceMonoOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot, detailGraphs, graphIndex)->CopyTo(seriesMonoIn);
    }
    if constexpr (Stereo)
    {
      seriesStereoIn.SanityCheck();
      for (int i = 0; i < processed; i++)
      {
        seriesOut.l.push_back(seriesStereoIn[0].Get(i));
        seriesOut.r.push_back(seriesStereoIn[1].Get(i));
      }
    }
    else
    {
      seriesMonoIn.SanityCheck();
      for (int i = 0; i < processed; i++)
        seriesOut.l.push_back(seriesMonoIn.Get(i));
    }
  }

  renderData.ProcessIndicators(renderState, detailGraphs, graphIndex, exchange, exchangeVoice, seriesOut);
  renderData.PostProcess(renderState, detailGraphs, graphIndex, exchange, exchangeVoice, seriesOut);
  renderData.ReleaseOnDemandBuffers(renderState, detailGraphs, graphIndex, exchange, exchangeVoice);
}

template <bool Global, bool Stereo, class Derived>
void
FBRenderModuleGraph(
  FBModuleGraphRenderData<Derived>& renderData, 
  bool detailGraphs, int graphIndex) 
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
  auto plotParams = renderData.plotParamsSelector(graphData, detailGraphs, graphIndex);
  int maxDspSampleCount = plotParams.sampleCount;
  if (maxDspSampleCount == 0)
    return;

  if (guiRenderType == FBGUIRenderType::Full)
  {
    if constexpr (Global)
    {
      auto moduleExchange = renderData.globalExchangeSelector(
        exchangeState, moduleProcState->moduleSlot, detailGraphs, graphIndex);
      moduleProcState->anyExchangeActive |= (moduleExchange->boolIsActive != 0);
      if (moduleExchange->boolIsActive != 0)
        maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->LengthSamples(detailGraphs, graphIndex));
    }
    else for (int v = 0; v < FBMaxVoices; v++)
    {
      auto moduleExchange = renderData.voiceExchangeSelector(
        exchangeState, v, moduleProcState->moduleSlot, detailGraphs, graphIndex);
      moduleProcState->anyExchangeActive |= (moduleExchange->boolIsActive != 0);
      if (moduleExchange->boolIsActive != 0)
        maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->LengthSamples(detailGraphs, graphIndex));
    }
  }

  float guiSampleRate;
  float guiSampleCount;
  auto hostExchange = renderState->ExchangeContainer()->Host();

  // Some vst3 hosts dont get this right.
  // See https://forums.steinberg.net/t/dataexchange-on-linux/917660/5.
  float hostBPM = hostExchange->bpm == 0.0f ? FBExchangeDefaultHostBPM : hostExchange->bpm;
  float hostSampleRate = hostExchange->sampleRate == 0.0f ? FBExchangeDefaultHostSampleRate : hostExchange->sampleRate;
  if (plotParams.autoSampleRate)
  {
    guiSampleCount = static_cast<float>(graphData->pixelWidth);
    guiSampleRate = hostSampleRate / (maxDspSampleCount / guiSampleCount);
  }
  else
  {
    guiSampleRate = plotParams.sampleRate;
    guiSampleCount = static_cast<float>(plotParams.sampleCount);
  }
  renderState->PrepareForRenderPrimary(guiSampleRate, hostBPM);
  if constexpr(!Global)
    renderState->PrepareForRenderPrimaryVoice();
  moduleProcState->renderType = FBRenderType::GraphPrimary;
  FBRenderModuleGraphSeries<Global, Stereo>(
    renderData, detailGraphs, graphIndex, false, -1,
    graphData->graphs[graphIndex].primarySeries);
  
  if (guiRenderType == FBGUIRenderType::Basic)
    return;

  renderState->PrepareForRenderExchange(hostExchange->noteMatrix);
  if constexpr (Global)
  {
    auto moduleExchange = renderData.globalExchangeSelector(
      exchangeState, moduleProcState->moduleSlot, detailGraphs, graphIndex);
    if (!moduleExchange->ShouldGraph(detailGraphs, graphIndex))
      return;
    float positionNormalized = moduleExchange->PositionNormalized(detailGraphs, graphIndex);
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
    FBRenderModuleGraphSeries<Global, Stereo>(renderData, detailGraphs, graphIndex, true, -1, secondary.points);
    secondary.marker = static_cast<int>(positionNormalized * secondary.points.l.size());
  } else for (int v = 0; v < FBMaxVoices; v++)
  {
    auto moduleExchange = renderData.voiceExchangeSelector(
      exchangeState, v, moduleProcState->moduleSlot, detailGraphs, graphIndex);
    if (!moduleExchange->ShouldGraph(detailGraphs, graphIndex))
      continue;
    renderState->PrepareForRenderExchangeVoice(v);
    float positionNormalized = moduleExchange->PositionNormalized(detailGraphs, graphIndex);
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
    FBRenderModuleGraphSeries<false, Stereo>(renderData, detailGraphs, graphIndex, true, v, secondary.points);
    secondary.marker = static_cast<int>(positionNormalized * secondary.points.l.size());
  }
}