#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/echo/FFEchoGraph.hpp>
#include <firefly_synth/modules/echo/FFEchoProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>

static bool
IsTapsOn(
  FBGraphRenderState* state,
  bool exchange, int exchangeVoice)
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFEchoParam::TapsOn, 0 } };
  return state->AudioParamBool(indices, exchange, exchangeVoice);
}

static bool
IsReverbOn(
  FBGraphRenderState* state,
  bool exchange, int exchangeVoice)
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFEchoParam::ReverbOn, 0 } };
  return state->AudioParamBool(indices, exchange, exchangeVoice);
}

static bool
IsFeedbackOn(
  FBGraphRenderState* state,
  bool exchange, int exchangeVoice)
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFEchoParam::FeedbackOn, 0 } };
  return state->AudioParamBool(indices, exchange, exchangeVoice);
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
    true, moduleProcState->input->sampleRate);
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
  FBParamTopoIndices indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  auto target = state->AudioParamList<FFGEchoTarget>(indices, false, -1);
  if (target == FFGEchoTarget::Off)
    return 0;

  indices = { { (int)FFModuleType::GEcho, 0 }, { (int)FFEchoParam::VOrderOrGOrder, 0 } };
  auto order = state->AudioParamList<FFEchoOrder>(indices, false, -1);
  if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Taps) && !IsTapsOn(state, exchange, exchangeVoice))
    return 0;
  if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Feedback) && !IsFeedbackOn(state, exchange, exchangeVoice))
    return 0;
  if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Reverb) && !IsReverbOn(state, exchange, exchangeVoice))
    return 0;

  auto* procState = moduleProcState->ProcAs<FFProcState>();
  auto& input = procState->dsp.global.gEcho.input;
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      int delaySamples = FBFixedBlockSamples * c;
      int inputSample = samplesProcessed[graphIndex] + s;
      float sawIn = FBToBipolar((inputSample - delaySamples) / (float)FBFixedBlockSamples);
      input[c].Set(s, delaySamples <= inputSample && inputSample <= FBFixedBlockSamples + delaySamples ? sawIn : 0.0f);
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
  FBParamTopoIndices paramIndices = { modIndices, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  auto target = renderState->AudioParamList<FFGEchoTarget>(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::VOrderOrGOrder, 0 } };
  auto order = renderState->AudioParamList<FFEchoOrder>(paramIndices, false, -1);
  bool on = target != FFGEchoTarget::Off;
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;

  int tapsOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Taps);
  FBRenderModuleGraph<true, true>(renderData, tapsOrder);
  graphData->graphs[tapsOrder].text = moduleName + " Taps";
  if(!on || !IsTapsOn(renderState, false, -1))
    graphData->graphs[tapsOrder].text += " Off";

  int feedbackOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Feedback);
  FBRenderModuleGraph<true, true>(renderData, feedbackOrder);
  graphData->graphs[feedbackOrder].text = moduleName + " Feedback";
  if (!on || !IsFeedbackOn(renderState, false, -1))
    graphData->graphs[feedbackOrder].text += " Off";

  int reverbOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Reverb);
  FBRenderModuleGraph<true, true>(renderData, reverbOrder);
  graphData->graphs[reverbOrder].text = moduleName + " Reverb";
  if (!on || !IsReverbOn(renderState, false, -1))
    graphData->graphs[reverbOrder].text += " Off";

  int allOrder = (int)FFEchoModule::Count;
  FBRenderModuleGraph<true, true>(renderData, allOrder);
  graphData->graphs[allOrder].text = moduleName;
  if (!on)
    graphData->graphs[allOrder].text += " Off";
}