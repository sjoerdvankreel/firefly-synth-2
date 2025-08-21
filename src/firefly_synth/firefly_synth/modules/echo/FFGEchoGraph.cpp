#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/echo/FFGEchoGraph.hpp>
#include <firefly_synth/modules/echo/FFGEchoProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>

static bool
IsFeedbackOn(
  FBGraphRenderState* state,
  bool exchange, int exchangeVoice)
{
  FBParamTopoIndices  indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::FeedbackType, 0 } };
  auto feedbackType = state->AudioParamList<FFGEchoFeedbackType>(indices, exchange, exchangeVoice);
  return feedbackType != FFGEchoFeedbackType::Off;
}

static bool 
IsAnyTapOn(
  FBGraphRenderState* state, 
  bool exchange, int exchangeVoice)
{
  bool result = false;
  for (int i = 0; i < FFGEchoTapCount; i++)
  {
    FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::TapOn, i } };
    result |= state->AudioParamBool(indices, exchange, exchangeVoice);
  }
  return result;
}

struct GEchoGraphRenderData final:
public FBModuleGraphRenderData<GEchoGraphRenderData>
{
  int totalSamples = {};
  std::array<int, 4> samplesProcessed = {};

  FFGEchoProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoPostProcess(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, int /*graphIndex*/)
{
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = data->pixelWidth;
  result.sampleRate = data->pixelWidth / FFGEchoPlotLengthSeconds;
  result.staticModuleIndex = (int)FFModuleType::GEcho;
  return result;
}

void
GEchoGraphRenderData::DoReleaseOnDemandBuffers(
  FBGraphRenderState* state, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).ReleaseOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(),
    state->ProcContainer());
}

void 
GEchoGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).AllocOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(), 
    state->ProcContainer(),
    moduleProcState->input->sampleRate);
  GetProcessor(*moduleProcState).BeginBlock(
    true, graphIndex, totalSamples, *moduleProcState);
}

FFGEchoProcessor&
GEchoGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.global.gEcho.processor.get();
}

int 
GEchoGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFGEchoParam::Target, 0 } };
  auto target = state->AudioParamList<FFGEchoTarget>(indices, false, -1);
  if (target == FFGEchoTarget::Off)
    return 0;

  if (graphIndex == 0 && !IsAnyTapOn(state, exchange, exchangeVoice))
    return 0;
  if (graphIndex == 1 && !IsFeedbackOn(state, exchange, exchangeVoice))
      return 0;

  auto* procState = moduleProcState->ProcAs<FFProcState>();
  auto& input = procState->dsp.global.gEcho.input;
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float sawIn = FBToBipolar(s / (float)FBFixedBlockSamples);
      input[c].Set(s, (samplesProcessed[graphIndex] + s) <= FBFixedBlockSamples ? sawIn : 0.0f);
    }
  
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).Process(*moduleProcState);
}

void
FFGEchoRenderGraph(FBModuleGraphComponentData* graphData)
{
  GEchoGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.totalSamples = PlotParams(graphData, -1).sampleCount;
  renderData.globalExchangeSelector = [](void const* exchangeState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gEcho[0]; };
  renderData.globalStereoOutputSelector = [](void const* procState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gEcho.output; };

  auto* renderState = graphData->renderState;
  FBTopoIndices modIndices = { (int)FFModuleType::GEcho, 0 };
  FBParamTopoIndices paramIndices = { modIndices, { (int)FFGEchoParam::Target, 0 } };
  auto target = renderState->AudioParamList<FFGEchoTarget>(paramIndices, false, -1);
  bool on = target != FFGEchoTarget::Off;
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;

  FBRenderModuleGraph<true, true>(renderData, 0);
  graphData->graphs[0].text = moduleName + " Taps";
  if(!on || !IsAnyTapOn(renderState, false, -1))
    graphData->graphs[0].text += " Off";

  FBRenderModuleGraph<true, true>(renderData, 1);
  graphData->graphs[1].text = moduleName + " Feedback";
  if (!on || !IsFeedbackOn(renderState, false, -1))
    graphData->graphs[1].text += " Off";

  FBRenderModuleGraph<true, true>(renderData, 3);
  graphData->graphs[3].text = moduleName;
  if (!on)
    graphData->graphs[3].text += " Off";
}