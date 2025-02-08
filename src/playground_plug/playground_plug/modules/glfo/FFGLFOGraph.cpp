#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/gui/shared/FBGUIGraphing.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <algorithm>

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  graphData->text = "OFF";
  auto renderState = graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  auto scalarState = renderState->ScalarContainer()->Raw();
  auto exchangeState = renderState->ExchangeContainer()->As<FFExchangeState>();
  auto const& moduleExchange = exchangeState->global.gLFO[moduleProcState->moduleSlot];

  if (!moduleExchange.active)
    return;
    
  FBModuleGraphRenderData<FFGLFOProcessor> renderData;
  renderData.graphData = graphData;
  renderData.globalOutputSelector = [](void const* procState, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };

  float guiSampleCount = (float)graphData->pixelWidth;
  float dspSampleCount = (float)moduleExchange.lengthSamples;
  float dspSampleRate = renderState->ExchangeContainer()->SampleRate();
  moduleProcState->sampleRate = dspSampleRate / (dspSampleCount / guiSampleCount);  

  renderState->PrepareForRenderPrimary();
  FBRenderModuleGraphSeries<true>(renderData, graphData->primarySeries);
  float durationSeconds = renderData.graphData->primarySeries.size() / moduleProcState->sampleRate;
  renderData.graphData->text = FBFormatFloat(durationSeconds, FBDefaultDisplayPrecision) + " Sec";
  
  renderState->PrepareForRenderExchange();  
  assert(moduleExchange.positionSamples < moduleExchange.lengthSamples);
  float positionNormalized = moduleExchange.positionSamples / (float)moduleExchange.lengthSamples;
  if (renderState->GlobalModuleExchangeStateEqualsPrimary((int)FFModuleType::GLFO, moduleProcState->moduleSlot))
  {
    graphData->primaryMarkers.push_back((int)(positionNormalized * graphData->primarySeries.size()));
    assert(graphData->primaryMarkers[graphData->primaryMarkers.size() - 1] < graphData->primarySeries.size());
    return;
  }

  auto& secondary = graphData->secondarySeries.emplace_back();
  FBRenderModuleGraphSeries<true>(renderData, secondary.points);
  secondary.marker = (int)(positionNormalized * secondary.points.size());
  assert(secondary.marker < secondary.points.size());
}