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
  // TODO figure out SR based on pixel width?
  float const sampleRate = 100.0f;
  FFModuleGraphRenderData<FFEnvProcessor> renderData;
  renderData.graphData = graphData;
  renderData.graphData->state->ModuleState().sampleRate = sampleRate;
  renderData.outputSelector = [](auto const& dspState, int moduleSlot) {
    return &dspState.voice[0].env[moduleSlot].output; };

  int maxPoints = 0;
  graphData->state->PrepareForRender(true, -1);
  FFRenderModuleGraph(renderData, graphData->primaryPoints);
  maxPoints = std::max(maxPoints, (int)graphData->primaryPoints.size());
  for(int v = 0; v < FBMaxVoices; v++)
    if (graphData->state->ExchangeContainer()->VoiceState()[v].state == FBVoiceState::Active)
    {
      int slot = renderData.graphData->state->ModuleState().moduleSlot;
      auto exchange = graphData->state->ExchangeState<FFExchangeState>();
      auto const& envExchange = exchange->voice[v].env[slot];
      if (envExchange.active && envExchange.positionSamples < envExchange.lengthSamples)
      {
        auto& secondary = graphData->secondaryData.emplace_back();
        graphData->state->PrepareForRender(false, v);
        FFRenderModuleGraph(renderData, secondary.points);
        maxPoints = std::max(maxPoints, (int)secondary.points.size());
        secondary.marker = (int)((envExchange.positionSamples / (float)envExchange.lengthSamples) * secondary.points.size());
      }
    }

  // todo less ugly
  graphData->primaryPoints.insert(graphData->primaryPoints.end(), maxPoints - graphData->primaryPoints.size(), 0.0f);
  for (int i = 0; i < graphData->secondaryData.size(); i++)
    graphData->secondaryData[i].points.insert(graphData->secondaryData[i].points.end(), maxPoints - graphData->secondaryData[i].points.size(), 0.0f);

  float durationSections = renderData.graphData->primaryPoints.size() / sampleRate;
  renderData.graphData->text = std::format("{:.3f}", durationSections) + " Sec";
}