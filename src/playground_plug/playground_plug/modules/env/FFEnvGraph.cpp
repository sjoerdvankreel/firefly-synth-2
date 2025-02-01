#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <format>

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

  graphData->state->PrepareForRender(true, -1);
  FFRenderModuleGraph(renderData, graphData->primaryPoints);
  for(int v = 0; v < FBMaxVoices; v++)
    if (graphData->state->ExchangeContainer()->VoiceActive()[v])
    {
      int slot = renderData.graphData->state->ModuleState().moduleSlot;
      auto exchange = graphData->state->ExchangeState<FFExchangeState>();
      if (exchange->voice[v].env[slot].active)
      {
        auto& secondary = graphData->secondaryData.emplace_back();
        graphData->state->PrepareForRender(false, v);
        FFRenderModuleGraph(renderData, secondary.points);
        secondary.marker = exchange->voice[v].env[v].position;
      }
    }

  float durationSections = renderData.graphData->primaryPoints.size() / sampleRate;
  renderData.graphData->text = std::format("{:.3f}", durationSections) + " Sec";
}