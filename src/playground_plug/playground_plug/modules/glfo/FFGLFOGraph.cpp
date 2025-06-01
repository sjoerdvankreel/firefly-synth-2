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
  void DoReset(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(bool exchange, int exchangeVoice, int graphIndex, FBModuleGraphPoints& points) {}
};

FFGLFOProcessor& 
GLFOGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.global.gLFO[state.moduleSlot].processor;
}

void 
GLFOGraphRenderData::DoReset(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).Reset(*moduleProcState);
}

int 
GLFOGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  return GetProcessor(*moduleProcState).Process(*moduleProcState);
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
  renderData.globalMonoOutputSelector = [](void const* procState, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };
  
  FBRenderModuleGraph<true, false>(renderData, 0);
  FBTopoIndices modIndices = { (int)FFModuleType::GLFO, graphData->renderState->ModuleProcState()->moduleSlot };
  FBParamTopoIndices paramIndices = { modIndices.index, modIndices.slot, (int)FFGLFOParam::On, 0 };
  graphData->graphs[0].text = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
  if (!graphData->renderState->AudioParamBool(paramIndices, false, -1))
    graphData->graphs[0].text += " OFF";
}