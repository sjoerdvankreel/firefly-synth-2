#pragma once

#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <vector>
#include <cassert>
#include <functional>

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

typedef std::function<FBFixedFloatArray const* (
  void const* procState, int moduleSlot)>
FBModuleGraphGlobalCVOutputSelector;
typedef std::function<FBFixedFloatAudioArray const* (
  void const* procState, int moduleSlot)>
  FBModuleGraphGlobalAudioOutputSelector;
typedef std::function<FBFixedFloatArray const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceCVOutputSelector;
typedef std::function<FBFixedFloatAudioArray const* (
  void const* procState, int voice, int moduleSlot)>
FBModuleGraphVoiceAudioOutputSelector;

typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int moduleSlot)>
FBModuleGraphGlobalExchangeSelector;    
typedef std::function<FBModuleProcExchangeState const* (
  void const* exchangeState, int voice, int moduleSlot)>
FBModuleGraphVoiceExchangeSelector;    

template <class Derived>
struct FBModuleGraphRenderData
{
  int staticModuleIndex = -1;
  FBModuleGraphComponentData* graphData = {};
  FBModuleGraphPlotParamsSelector plotParamsSelector = {};
  FBModuleGraphVoiceExchangeSelector voiceExchangeSelector = {};
  FBModuleGraphGlobalExchangeSelector globalExchangeSelector = {};
  FBModuleGraphVoiceCVOutputSelector voiceCVOutputSelector = {};
  FBModuleGraphGlobalCVOutputSelector globalCVOutputSelector = {};
  FBModuleGraphVoiceAudioOutputSelector voiceAudioOutputSelector = {};
  FBModuleGraphGlobalAudioOutputSelector globalAudioOutputSelector = {};

  void Reset(FBModuleProcState& state) { static_cast<Derived*>(this)->Reset(state); }
  int Process(FBModuleProcState& state) { return static_cast<Derived*>(this)->Process(state); }
  void BeginVoice(FBModuleProcState& state) { static_cast<Derived*>(this)->BeginVoice(state); }
};

template <bool Global, bool Audio, class Derived> 
void 
FBRenderModuleGraphSeries(
  FBModuleGraphRenderData<Derived>& renderData,
  int releaseAt, FBModuleGraphPoints& seriesOut)
{
  seriesOut.l.clear();
  seriesOut.r.clear();
  
  FBFixedFloatArray seriesCVIn;
  FBFixedFloatAudioArray seriesAudioIn;

  bool released = false;
  int processed = FBFixedBlockSamples;
  bool shouldRelease = releaseAt >= 0;
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
    if (shouldRelease && !released && releaseAt < FBFixedBlockSamples)
    {
      released = true;
      moduleProcState->input->note->push_back(FBDetailMakeNoteC4Off(releaseAt));
    }
    processed = renderData.Process(*moduleProcState);
    if (shouldRelease && !released)
      releaseAt -= processed;
    if constexpr (Global)
    {
      if constexpr(Audio)
        renderData.globalAudioOutputSelector(
          moduleProcState->procRaw,
          moduleSlot)->StoreToFloatArray(seriesAudioIn);
      else
        renderData.globalCVOutputSelector(
          moduleProcState->procRaw,
          moduleSlot)->StoreToFloatArray(seriesCVIn);
    }
    else
    {
      if constexpr (Audio)
        renderData.voiceAudioOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot)->StoreToFloatArray(seriesAudioIn);
      else
        renderData.voiceCVOutputSelector(
          moduleProcState->procRaw,
          moduleProcState->voice->slot,
          moduleSlot)->StoreToFloatArray(seriesCVIn);
    }
    if constexpr (Audio)
      for (int i = 0; i < processed; i++)
      {
        seriesOut.l.push_back(seriesAudioIn[0][i]);
        seriesOut.r.push_back(seriesAudioIn[1][i]);
      }
    else
      for (int i = 0; i < processed; i++)
        seriesOut.l.push_back(seriesCVIn[i]);
  }
}

template <bool Global, bool Audio, class Derived>
void
FBRenderModuleGraph(FBModuleGraphRenderData<Derived>& renderData)
{
  auto graphData = renderData.graphData;
  auto renderState = graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  auto exchangeState = renderState->ExchangeContainer()->Raw();

  assert(renderData.graphData != nullptr);
  assert(renderData.staticModuleIndex != -1);
  assert(renderData.plotParamsSelector != nullptr);

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
  auto hostExchange = renderState->ExchangeContainer()->Host();
  float guiSampleCount = static_cast<float>(graphData->pixelWidth);
  float dspSampleRate = renderState->ExchangeContainer()->Host()->sampleRate;
  float guiSampleRate = hostExchange->sampleRate / (maxDspSampleCount / guiSampleCount);
  if(plotParams.releaseAt != -1)
    guiReleaseAt = static_cast<int>(std::round(guiSampleRate * plotParams.releaseAt / dspSampleRate));
  renderState->PrepareForRenderPrimary(guiSampleRate, hostExchange->bpm);
  if constexpr(!Global)
    renderState->PrepareForRenderPrimaryVoice();
  moduleProcState->renderType = FBRenderType::GraphPrimary;
  FBRenderModuleGraphSeries<Global, Audio>(renderData, guiReleaseAt, graphData->primarySeries);
  float guiDurationSeconds = renderData.graphData->primarySeries.l.size() / moduleProcState->input->sampleRate;
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
        static_cast<int>(positionNormalized * graphData->primarySeries.l.size()));
      return;
    }
    moduleProcState->renderType = FBRenderType::GraphExchange;
    auto& secondary = graphData->secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<Global, Audio>(renderData, -1, secondary.points);
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
      graphData->primaryMarkers.push_back(
        static_cast<int>(positionNormalized * graphData->primarySeries.l.size()));
      continue;
    }
    moduleProcState->renderType = FBRenderType::GraphExchange;
    auto& secondary = graphData->secondarySeries.emplace_back();
    FBRenderModuleGraphSeries<false, Audio>(renderData, -1, secondary.points);
    secondary.marker = static_cast<int>(positionNormalized * secondary.points.l.size());
  }
}