#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGraph.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessorProcess.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

struct GlobalUniGraphRenderData final :
public FBModuleGraphRenderData<GlobalUniGraphRenderData>
{
  FFGlobalUniProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/) {}
  void DoPostProcess(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoProcessIndicators(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, int /*graphIndex*/)
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.sampleCount = data->pixelWidth * 2;
  result.staticModuleIndex = (int)FFModuleType::GlobalUni;
  return result;
}

int
GlobalUniGraphRenderData::DoProcess(
  FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  return FBFixedBlockSamples;
}

FFGlobalUniProcessor&
GlobalUniGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.global.globalUni.processor;
}

void
GlobalUniGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).BeginBlock(*moduleProcState);
}

void
FFGlobalUniRenderGraph(FBModuleGraphComponentData* graphData)
{
  GlobalUniGraphRenderData renderData = {};
  graphData->bipolar = false;
  graphData->drawClipBoundaries = false;
  graphData->skipDrawOnEqualsPrimary = true;
  graphData->drawMarkersSelector = [](int) { return true; };
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.globalExchangeSelector = [](void const* exchangeState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.globalUni[0]; };
  renderData.globalMonoOutputSelector = [](void const* procState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.globalUni.fakeGraphOutput; };

  for(int i = 0; i < (int)FFGlobalUniTarget::Count; i++)
    FBRenderModuleGraph<true, false>(renderData, i);
}