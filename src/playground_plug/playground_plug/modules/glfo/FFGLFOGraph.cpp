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
  // todo probably need to share some of this

  FBModuleGraphRenderData<FFGLFOProcessor> renderData;
  auto renderState = graphData->renderState;
  auto moduleProcState = renderState->ModuleProcState();
  auto scalarState = renderState->ScalarContainer()->Raw();
  auto exchangeState = renderState->ExchangeContainer()->As<FFExchangeState>();
  auto const& moduleExchange = exchangeState->global.gLFO[moduleProcState->moduleSlot];

  float guiSampleCount = (float)graphData->pixelWidth;
  float dspSampleCount = renderData.processor.StaticLengthSamples(
    scalarState, moduleProcState->moduleSlot, exchangeState->sampleRate);
  float dspSampleRate = renderState->ExchangeContainer()->SampleRate();
  if (moduleExchange.active)
    dspSampleCount = (float)moduleExchange.lengthSamples;
  moduleProcState->sampleRate = dspSampleRate / (dspSampleCount / guiSampleCount);

  renderData.graphData = graphData;
  renderData.globalOutputSelector = [](void const* procState, int slot) { 
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };

  renderState->PrepareForRenderPrimary();
  FBRenderModuleGraphSeries<true>(renderData, graphData->primarySeries);

  renderState->PrepareForRenderExchange();

  
  assert(moduleExchange.positionSamples < moduleExchange.lengthSamples);
  float positionNormalized = moduleExchange.positionSamples / (float)moduleExchange.lengthSamples;
  if (renderState->GlobalModuleExchangeStateEqualsPrimary((int)FFModuleType::GLFO, moduleProcState->moduleSlot))
  {
    graphData->primaryMarkers.push_back((int)(positionNormalized * graphData->primarySeries.size()));
    assert(graphData->primaryMarkers[graphData->primaryMarkers.size() - 1] < graphData->primarySeries.size());
    return;
  }

  renderState->PrepareForRenderExchange();
  auto& secondary = graphData->secondarySeries.emplace_back();
  FBRenderModuleGraphSeries<true>(renderData, secondary.points);
  secondary.marker = (int)(positionNormalized * secondary.points.size());
  assert(secondary.marker < secondary.points.size());

  float durationSeconds = renderData.graphData->primarySeries.size() / moduleProcState->sampleRate;
  renderData.graphData->text = FBFormatFloat(durationSeconds, FBDefaultDisplayPrecision) + " Sec";
}