#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

static FBModuleGraphPlotParams
PlotParams(FBGraphRenderState const* state)
{
  FBModuleGraphPlotParams result = {};
  result.releaseAt = -1;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Proc()->sampleRate;
  result.samples = state->AudioParamLinearFreqSamples({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 }, sampleRate);
  return result;
}

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  FBModuleGraphRenderData<FFGLFOProcessor> renderData = {};
  graphData->drawMarkers = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::GLFO;
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gLFO[slot]; };
  renderData.globalCVOutputSelector = [](void const* procState, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };
  FBRenderModuleGraph<true, false>(renderData);
}