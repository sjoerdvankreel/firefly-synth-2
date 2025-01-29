#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <format>

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  // TODO figure out SR based on pixel width?
  float const sampleRate = 100.0f;
  FFModuleGraphRenderData<FFEnvProcessor> renderData;
  renderData.graphData = graphData;
  renderData.graphData->state->moduleState.sampleRate = sampleRate;
  renderData.outputSelector = [](auto const& dspState, int moduleSlot) {
    return &dspState.voice[0].env[moduleSlot].output; };
  
  FFRenderModuleGraph(renderData);
  float durationSections = renderData.graphData->points.size() / sampleRate;
  renderData.graphData->text = std::format("{:.2f}", durationSections) + " Sec";
}