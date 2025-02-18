#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>

#include <algorithm>

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  FBModuleGraphRenderData<FFGLFOProcessor> renderData = {};
  renderData.graphData = graphData;
  renderData.staticModuleIndex = (int)FFModuleType::GLFO;
  renderData.plotLengthSelector = [&](FBGraphRenderState const* state) {
    return renderData.processor.PlotLengthSamples(state); };
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gLFO[slot]; };
  renderData.globalOutputSelector = [](void const* procState, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };
  FBRenderModuleGraph<true>(renderData);
}