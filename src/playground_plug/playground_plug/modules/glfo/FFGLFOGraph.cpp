#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <algorithm>

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  // todo probably need to share some of this

  float const sampleRate = 100.0f;
  auto renderState = graphData->renderState;
  auto& moduleState = renderState->ModuleState();
  moduleState.sampleRate = sampleRate;
  int moduleSlot = moduleState.moduleSlot;

  FFModuleGraphRenderData<FFGLFOProcessor> renderData;
  renderData.graphData = graphData;
  renderData.globalOutputSelector = [](auto const& dsp, int slot) { 
    return &dsp.global.gLFO[slot].output; };

  renderState->PrepareForRenderPrimary();
  FFRenderModuleGraph<true>(renderData, graphData->primarySeries);
  int maxPoints = (int)graphData->primarySeries.size();

  renderState->PrepareForRenderExchange();
  auto exchangeState = renderState->ExchangeState<FFExchangeState>();
  auto const& gLFOExchange = exchangeState->global.gLFO[moduleSlot];

  if (!gLFOExchange.active)
    return;
  assert(gLFOExchange.cyclePositionSamples < gLFOExchange.cycleLengthSamples);
  float positionNormalized = gLFOExchange.cyclePositionSamples / (float)gLFOExchange.cycleLengthSamples;
  if (renderState->GlobalModuleExchangeStateEqualsPrimary((int)FFModuleType::GLFO, moduleSlot))
  {
    graphData->primaryMarkers.push_back((int)(positionNormalized * graphData->primarySeries.size()));
    assert(graphData->primaryMarkers[graphData->primaryMarkers.size() - 1] < graphData->primarySeries.size());
    return;
  }

  auto& secondary = graphData->secondarySeries.emplace_back();
  FFRenderModuleGraph<true>(renderData, secondary.points);
  maxPoints = std::max(maxPoints, (int)secondary.points.size());
  secondary.marker = (int)(positionNormalized * secondary.points.size());
  assert(secondary.marker < secondary.points.size());

  int zeroFillCount = maxPoints - (int)graphData->primarySeries.size();
  graphData->primarySeries.insert(graphData->primarySeries.end(), zeroFillCount, 0.0f);
  for (int i = 0; i < graphData->secondarySeries.size(); i++)
  {
    auto& secondaryPoints = graphData->secondarySeries[i].points;
    zeroFillCount = maxPoints - (int)secondaryPoints.size();
    secondaryPoints.insert(secondaryPoints.end(), zeroFillCount, 0.0f);
  }

  float durationSections = renderData.graphData->primarySeries.size() / sampleRate;
  renderData.graphData->text = FBFormatFloat(durationSections, FBDefaultDisplayPrecision) + " Sec";
}