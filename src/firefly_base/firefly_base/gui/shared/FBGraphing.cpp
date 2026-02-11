#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

static void 
FBRenderModuleGraphSeries(
  FBModuleGraphProcessor* processor, bool stereo, 
  FBModuleGraphStateParams const& stateParams,
  FBModuleGraphProcessParams const& processParams,
  FBModuleProcExchangeStateBase const* exchangeState, 
  FBModuleGraphPoints& seriesOut)
{
  seriesOut.l.clear();
  seriesOut.r.clear();
  seriesOut.verticalIndicators.clear();
  
  FBSArray<float, FBFixedBlockSamples> seriesMonoIn = {};
  FBSArray2<float, FBFixedBlockSamples, 2> seriesStereoIn = {};

  int processed = FBFixedBlockSamples;
  auto renderState = processor->ComponentData()->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  moduleProcState->input->noteEvents->clear();

  processor->BeginVoiceOrBlock(renderState, processParams);
  while (processed == FBFixedBlockSamples)
  {
    processed = processor->Process(renderState, processParams);
    if (stereo)
      processor->StereoOutput(moduleProcState->procRaw, stateParams)->CopyTo(seriesStereoIn);
    else
      processor->MonoOutput(moduleProcState->procRaw, stateParams)->CopyTo(seriesMonoIn);
    if (stereo)
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

  auto& graphData = processor->ComponentData()->graphs[processParams.graphIndex];
  processor->ProcessIndicators(renderState, processParams, seriesOut);
  processor->PostProcess(renderState, graphData, processParams, seriesOut);
  processor->ReleaseOnDemandBuffers(renderState, processParams);
  if (processParams.exchange)
    processor->ProcessExchangeState(renderState, graphData, processParams, exchangeState);
}

void
FBRenderModuleGraph(
  FBModuleGraphProcessor* processor,
  bool global, bool stereo,
  bool detailGraphs, int graphIndex)
{
  auto componentData = processor->ComponentData();
  auto renderState = componentData->renderState;
  auto guiRenderType = componentData->guiRenderType;
  auto moduleProcState = renderState->ModuleProcState();
  auto exchangeState = renderState->ExchangeContainer()->Raw();

  moduleProcState->anyExchangeActive = false;
  componentData->graphs[graphIndex].anyExchangeActive = false;
  auto plotParams = processor->PlotParams(detailGraphs, graphIndex);
  int maxDspSampleCount = plotParams.sampleCount;
  if (maxDspSampleCount == 0)
    return;

  FBModuleGraphStateParams stateParams = {};
  stateParams.voice = -1;
  stateParams.global = global;
  stateParams.graphIndex = graphIndex;
  stateParams.detailGraphs = detailGraphs;
  stateParams.moduleSlot = moduleProcState->moduleSlot;
  if (guiRenderType == FBGUIRenderType::Full)
  {
    if (global)
    {
      auto moduleExchange = processor->ExchangeState(exchangeState, stateParams);
      moduleProcState->anyExchangeActive |= (moduleExchange->boolIsActive != 0);
      componentData->graphs[graphIndex].anyExchangeActive |= (moduleExchange->boolIsActive != 0);
      if (moduleExchange->boolIsActive != 0)
        maxDspSampleCount = std::max(maxDspSampleCount, moduleExchange->LengthSamples(detailGraphs, graphIndex));
    }
    else for (int v = 0; v < FBMaxVoices; v++)
    {
      stateParams.voice = v;
      auto moduleExchange = processor->ExchangeState(exchangeState, stateParams);
      moduleProcState->anyExchangeActive |= (moduleExchange->boolIsActive != 0);
      componentData->graphs[graphIndex].anyExchangeActive |= (moduleExchange->boolIsActive != 0);
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
    guiSampleCount = static_cast<float>(componentData->pixelWidth);
    guiSampleRate = hostSampleRate / (maxDspSampleCount / guiSampleCount);
  }
  else
  {
    guiSampleRate = plotParams.sampleRate;
    guiSampleCount = static_cast<float>(plotParams.sampleCount);
  }
  renderState->PrepareForRenderPrimary(guiSampleRate, hostBPM);
  if (!global)
    renderState->PrepareForRenderPrimaryVoice();
  moduleProcState->renderType = FBRenderType::GraphPrimary;

  stateParams.voice = global? -1: 0;
  FBModuleGraphProcessParams processParams = {};
  processParams.exchange = false;
  processParams.exchangeVoice = -1;
  processParams.graphIndex = graphIndex;
  processParams.detailGraphs = detailGraphs;
  processParams.guiSampleRate = guiSampleRate;
  processParams.hostSampleRate = hostSampleRate;
  FBRenderModuleGraphSeries(processor, stereo, stateParams, processParams, nullptr, componentData->graphs[graphIndex].primarySeries);
  
  if (guiRenderType == FBGUIRenderType::Basic)
    return;

  auto& graphData = componentData->graphs[graphIndex];
  renderState->PrepareForRenderExchange(hostExchange->noteMatrix);
  processParams.exchange = true;
  if (global)
  {
    auto moduleExchange = processor->ExchangeState(exchangeState, stateParams);
    if (!moduleExchange->ShouldGraph(detailGraphs, graphIndex))
      return;
    bool displayMarker = true;
    float positionNormalized = moduleExchange->PositionNormalized(detailGraphs, graphIndex);
    processor->PostProcessMarker(renderState, componentData->graphs[graphIndex], processParams, positionNormalized, displayMarker);
    if (componentData->skipDrawOnEqualsPrimary &&
      renderState->GlobalModuleExchangeStateEqualsPrimary(
      plotParams.staticModuleIndex, moduleProcState->moduleSlot))
    {
      if(displayMarker)
        componentData->graphs[graphIndex].primaryMarkers.push_back(
          static_cast<int>(positionNormalized * graphData.primarySeries.l.size()));
      return;
    }
    moduleProcState->renderType = FBRenderType::GraphExchange;
    auto& secondary = graphData.secondarySeries.emplace_back();
    FBRenderModuleGraphSeries(processor, stereo, stateParams, processParams, moduleExchange, secondary.points);
    secondary.marker = !displayMarker? -1: static_cast<int>(positionNormalized * secondary.points.l.size());
  } else
  {
    // Render voices from oldest to newest.
    // This makes it easier to just continuously overwrite metadata like main/subtext.
    auto const* voices = &renderState->ExchangeContainer()->Voices();
    std::vector<int> voiceIndicesByAge = {};
    for (int i = 0; i < FBMaxVoices; i++)
      voiceIndicesByAge.push_back(i);
    std::sort(voiceIndicesByAge.begin(), voiceIndicesByAge.end(), [voices](int l, int r) { return (*voices)[l].num < (*voices)[r].num; });

    for (int i = 0; i < FBMaxVoices; i++)
    {
      int v = voiceIndicesByAge[i];
      stateParams.voice = v;
      processParams.exchangeVoice = v;
      auto moduleExchange = processor->ExchangeState(exchangeState, stateParams);
      if (!moduleExchange->ShouldGraph(detailGraphs, graphIndex))
        continue;
      renderState->PrepareForRenderExchangeVoice(v);
      bool displayMarker = true;
      float positionNormalized = moduleExchange->PositionNormalized(detailGraphs, graphIndex);
      processor->PostProcessMarker(renderState, componentData->graphs[graphIndex], processParams, positionNormalized, displayMarker);
      if (componentData->skipDrawOnEqualsPrimary &&
        renderState->VoiceModuleExchangeStateEqualsPrimary(
          v, plotParams.staticModuleIndex, moduleProcState->moduleSlot))
      {
        if(displayMarker)
          graphData.primaryMarkers.push_back(static_cast<int>(positionNormalized * graphData.primarySeries.l.size()));
        continue;
      }
      moduleProcState->renderType = FBRenderType::GraphExchange;
      auto& secondary = graphData.secondarySeries.emplace_back();
      FBRenderModuleGraphSeries(processor, stereo, stateParams, processParams, moduleExchange, secondary.points);
      secondary.marker = !displayMarker? -1: static_cast<int>(positionNormalized * secondary.points.l.size());
    }
  }
}