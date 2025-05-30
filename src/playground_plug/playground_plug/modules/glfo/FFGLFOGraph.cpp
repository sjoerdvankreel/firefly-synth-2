#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct GLFOGraphRenderData final:
public FBModuleGraphRenderData<GLFOGraphRenderData>
{
  FFGLFOProcessor& GetProcessor(FBModuleProcState& state);
  void DoReset(FBModuleProcState& state) { GetProcessor(state).Reset(state); }
  int DoProcess(FBModuleProcState& state) { return GetProcessor(state).Process(state); }
  void DoProcessIndicators(bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

FFGLFOProcessor& 
GLFOGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.global.gLFO[state.moduleSlot].processor;
}

static int
PlotSamples(FBModuleGraphComponentData const* data)
{
  auto const* state = data->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  return state->AudioParamLinearFreqSamples({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 }, false, -1, sampleRate);
}

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  GLFOGraphRenderData renderData = {};
  graphData->drawMarkers = true;
  renderData.graphData = graphData;
  renderData.plotSamplesSelector = PlotSamples;
  renderData.staticModuleIndex = (int)FFModuleType::GLFO;
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gLFO[slot]; };
  renderData.globalCVOutputSelector = [](void const* procState, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };
  FBTopoIndices indices = { (int)FFModuleType::GLFO, graphData->renderState->ModuleProcState()->moduleSlot };
  graphData->graphs[0].moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(indices)->name;
  FBRenderModuleGraph<true, false>(renderData, 0);
}