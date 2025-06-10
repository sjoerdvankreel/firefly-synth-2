#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/glfo/FFGLFOTopo.hpp>
#include <firefly_synth/modules/glfo/FFGLFOGraph.hpp>
#include <firefly_synth/modules/glfo/FFGLFOProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct GLFOGraphRenderData final:
public FBModuleGraphRenderData<GLFOGraphRenderData>
{
  FFGLFOProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrReset(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(bool exchange, int exchangeVoice, int graphIndex, FBModuleGraphPoints& points) {}
  void DoPostProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

FFGLFOProcessor& 
GLFOGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.global.gLFO[state.moduleSlot].processor;
}

void 
GLFOGraphRenderData::DoBeginVoiceOrReset(
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

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data)
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;

  auto const* state = data->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  result.sampleCount = state->AudioParamLinearFreqSamples({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 }, false, -1, sampleRate);
  return result;
}

void
FFGLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  GLFOGraphRenderData renderData = {};
  graphData->drawMarkers = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
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