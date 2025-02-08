#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <algorithm>

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  // todo probably need to share some of this

  auto renderState = graphData->renderState;
  auto& moduleState = renderState->ModuleState();
  auto exchangeState = renderState->ExchangeState<FFExchangeState>();
  auto const& moduleExchange = exchangeState->global.gLFO[moduleState.moduleSlot];
  if (!moduleExchange.active)
    return;

  float guiSampleCount = (float)graphData->pixelWidth;
  float dspSampleCount = (float)moduleExchange.lengthSamples;
  float dspSampleRate = renderState->ExchangeContainer()->SampleRate();
  moduleState.sampleRate = dspSampleRate / (dspSampleCount / guiSampleCount);

  FBModuleGraphRenderData<FFGLFOProcessor> renderData;
  renderData.graphData = graphData;
  renderData.globalOutputSelector = [](void const* procState, int slot) { 
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };

  renderState->PrepareForRenderPrimary();
  FBRenderModuleGraphSeries<true>(renderData, graphData->primarySeries);

  renderState->PrepareForRenderExchange();

  
  assert(moduleExchange.positionSamples < moduleExchange.lengthSamples);
  float positionNormalized = moduleExchange.positionSamples / (float)moduleExchange.lengthSamples;
  if (renderState->GlobalModuleExchangeStateEqualsPrimary((int)FFModuleType::GLFO, moduleState.moduleSlot))
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

  float durationSeconds = renderData.graphData->primarySeries.size() / moduleState.sampleRate;
  renderData.graphData->text = FBFormatFloat(durationSeconds, FBDefaultDisplayPrecision) + " Sec";
}