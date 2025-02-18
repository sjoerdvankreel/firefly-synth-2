#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

static int
PlotLengthSamples(FBGraphRenderState const* state)
{
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Proc()->sampleRate;
  return state->AudioParamLinearFreqSamples({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 }, sampleRate);
}

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  FBModuleGraphRenderData<FFGLFOProcessor> renderData = {};
  renderData.graphData = graphData;
  renderData.plotLengthSelector = PlotLengthSamples;
  renderData.staticModuleIndex = (int)FFModuleType::GLFO;
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gLFO[slot]; };
  renderData.globalOutputSelector = [](void const* procState, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };
  FBRenderModuleGraph<true>(renderData);
}