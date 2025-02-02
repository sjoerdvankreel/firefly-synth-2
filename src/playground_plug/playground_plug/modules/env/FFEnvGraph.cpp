#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <format>
#include <algorithm>

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  // todo probably need to share some of this

  float const sampleRate = 100.0f;
  auto renderState = graphData->renderState;
  auto& moduleState = renderState->ModuleState();
  auto exchangeState = renderState->ExchangeState<FFExchangeState>();
  int moduleSlot = moduleState.moduleSlot;
  auto const& exchangeVoiceState = renderState->ExchangeContainer()->VoiceState();
  moduleState.sampleRate = sampleRate;

  FFModuleGraphRenderData<FFEnvProcessor> renderData;
  renderData.graphData = graphData;
  renderData.outputSelector = [](auto const& dsp, int v, int slot) { 
    return &dsp.voice[v].env[slot].output; };

  renderState->PrepareForRenderPrimary();
  FFRenderModuleGraph(renderData, graphData->primarySeries);
  int maxPoints = (int)graphData->primarySeries.size();

  renderState->PrepareForRenderExchange();
  for (int v = 0; v < FBMaxVoices; v++)
  {
    if (exchangeVoiceState[v].state != FBVoiceState::Active)
      continue;
    renderState->PrepareForRenderExchangeVoice(v);
    auto const& envExchange = exchangeState->voice[v].env[moduleSlot];
    if (!envExchange.active || envExchange.positionSamples >= envExchange.lengthSamples)
      continue;
    float positionNormalized = envExchange.positionSamples / (float)envExchange.lengthSamples;
    if (renderState->VoiceModuleExchangeStateEqualsPrimary(v, (int)FFModuleType::Env, moduleSlot))
    {
      graphData->primaryMarkers.push_back((int)(positionNormalized * graphData->primarySeries.size()));
      continue;
    }
    auto& secondary = graphData->secondarySeries.emplace_back();
    FFRenderModuleGraph(renderData, secondary.points);
    maxPoints = std::max(maxPoints, (int)secondary.points.size());
    secondary.marker = (int)(positionNormalized * secondary.points.size());
  }

  int zeroFillCount = maxPoints - (int)graphData->primarySeries.size();
  graphData->primarySeries.insert(graphData->primarySeries.end(), zeroFillCount, 0.0f);
  for (int i = 0; i < graphData->secondarySeries.size(); i++)
  {
    auto& secondaryPoints = graphData->secondarySeries[i].points;
    zeroFillCount = maxPoints - (int)secondaryPoints.size();
    secondaryPoints.insert(secondaryPoints.end(), zeroFillCount, 0.0f);
  }

  float durationSections = renderData.graphData->primarySeries.size() / sampleRate;
  renderData.graphData->text = std::format("{:.3f}", durationSections) + " Sec";
}