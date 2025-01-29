#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  FFModuleGraphRenderData<FFEnvProcessor> renderData;
  renderData.graphData = graphData;
  renderData.graphData->text = "ENV";
  renderData.graphData->state->moduleState.sampleRate = 100;
  renderData.outputSelector = [](auto const& dspState, int moduleSlot) {
    return &dspState.voice[0].env[moduleSlot].output; };
  FFRenderModuleGraph(renderData);
}