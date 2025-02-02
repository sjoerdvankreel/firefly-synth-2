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
  // TODO tidy up
  float const sampleRate = 100.0f;
  FFModuleGraphRenderData<FFEnvProcessor> renderData;
  renderData.graphData = graphData;
  renderData.graphData->state->ModuleState().sampleRate = sampleRate;
  renderData.outputSelector = [](auto const& dspState, int voice, int moduleSlot) {
    return &dspState.voice[voice].env[moduleSlot].output; };

  graphData->state->PrepareForRenderPrimary();
  FFRenderModuleGraph(renderData, graphData->primarySeries);
  int maxPoints = (int)graphData->primarySeries.size();
  graphData->state->PrepareForRenderExchange();
  for(int v = 0; v < FBMaxVoices; v++)
    if (graphData->state->ExchangeContainer()->VoiceState()[v].state == FBVoiceState::Active)
    {
      graphData->state->PrepareForRenderExchangeVoice(v);
      int slot = renderData.graphData->state->ModuleState().moduleSlot;
      auto exchange = graphData->state->ExchangeState<FFExchangeState>();
      auto const& envExchange = exchange->voice[v].env[slot];
      if (envExchange.active && envExchange.positionSamples < envExchange.lengthSamples)
      {
        auto& secondary = graphData->secondarySeries.emplace_back();
        FFRenderModuleGraph(renderData, secondary.points);
        maxPoints = std::max(maxPoints, (int)secondary.points.size());
        secondary.marker = (int)((envExchange.positionSamples / (float)envExchange.lengthSamples) * secondary.points.size());
      }
    }

  // todo less ugly
  graphData->primarySeries.insert(graphData->primarySeries.end(), maxPoints - graphData->primarySeries.size(), 0.0f);
  for (int i = 0; i < graphData->secondarySeries.size(); i++)
    graphData->secondarySeries[i].points.insert(graphData->secondarySeries[i].points.end(), maxPoints - graphData->secondarySeries[i].points.size(), 0.0f);

  float durationSections = renderData.graphData->primarySeries.size() / sampleRate;
  renderData.graphData->text = std::format("{:.3f}", durationSections) + " Sec";
}