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
  FBGraphRenderState* state, bool global,
  bool exchange, int exchangeVoice)
{
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::TapsOn, 0 } };
  return state->AudioParamBool(indices, exchange, exchangeVoice);
}

static bool
IsReverbOn(
  FBGraphRenderState* state, bool global,
  bool exchange, int exchangeVoice)
{
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::ReverbOn, 0 } };
  return state->AudioParamBool(indices, exchange, exchangeVoice);
}

static bool
IsFeedbackOn(
  FBGraphRenderState* state, bool global,
  bool exchange, int exchangeVoice)
{
  auto moduleType = global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::FeedbackOn, 0 } };
  return state->AudioParamBool(indices, exchange, exchangeVoice);
}

template <bool Global>
struct EchoGraphRenderData final:
public FBModuleGraphRenderData<EchoGraphRenderData<Global>>
{
  int totalSamples = {};
  std::array<int, 4> samplesProcessed = {};

  FFEchoProcessor<Global>& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoPostProcess(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, bool global, int /*graphIndex*/)
{
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = data->pixelWidth;
  result.sampleRate = data->pixelWidth / FFEchoPlotLengthSeconds;
  result.staticModuleIndex = (int)(global ? FFModuleType::GEcho : FFModuleType::VEcho);
  return result;
}

template <bool Global>
void
EchoGraphRenderData<Global>::DoReleaseOnDemandBuffers(
  FBGraphRenderState* state, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).ReleaseOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(),
    state->ProcContainer());
}

template <bool Global>
void 
EchoGraphRenderData<Global>::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).AllocOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(), 
    state->ProcContainer(),
    true, moduleProcState->input->sampleRate);
  GetProcessor(*moduleProcState).BeginVoiceOrBlock(
    true, graphIndex, totalSamples, *moduleProcState);
}

template <bool Global>
FFEchoProcessor<Global>&
EchoGraphRenderData<Global>::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *FFSelectDualState<Global>(
    [procState] { return procState->dsp.global.gEcho.processor.get(); },
    [procState, &state] { return procState->dsp.voice[state.voice->slot].vEcho.processor.get(); });
}

template <bool Global>
int 
EchoGraphRenderData<Global>::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  auto moduleType = Global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  auto target = state->AudioParamList<int>(indices, false, -1);
  if (target == 0)
    return 0;

  indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } };
  auto order = state->AudioParamList<FFEchoOrder>(indices, false, -1);
  if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Taps) && !IsTapsOn(state, Global, exchange, exchangeVoice))
    return 0;
  if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Feedback) && !IsFeedbackOn(state, Global, exchange, exchangeVoice))
    return 0;
  if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Reverb) && !IsReverbOn(state, Global, exchange, exchangeVoice))
    return 0;

  auto* procState = moduleProcState->ProcAs<FFProcState>();
  auto& input = *FFSelectDualState<Global>(
    [procState]() { return &procState->dsp.global.gEcho.input; },
    [procState, moduleProcState]() { return &procState->dsp.voice[moduleProcState->voice->slot].vEcho.input; });
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      int delaySamples = FBFixedBlockSamples * c;
      int inputSample = samplesProcessed[graphIndex] + s;
      float sawIn = FBToBipolar((inputSample - delaySamples) / (float)FBFixedBlockSamples);
      input[c].Set(s, delaySamples <= inputSample && inputSample <= FBFixedBlockSamples + delaySamples ? sawIn : 0.0f);
    }
  
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).Process(*moduleProcState, -1);
}

template <bool Global>
void
FFEchoRenderGraph(FBModuleGraphComponentData* graphData)
{
  EchoGraphRenderData<Global> renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = [](auto graphData, int graphIndex) { return PlotParams(graphData, Global, graphIndex); };
  renderData.totalSamples = PlotParams(graphData, Global, -1).sampleCount;
  renderData.globalExchangeSelector = [](void const* exchangeState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gEcho[0]; };
  renderData.globalStereoOutputSelector = [](void const* procState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gEcho.output; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].vEcho[0]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].vEcho.output; };

  auto* renderState = graphData->renderState;
  auto moduleType = Global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBTopoIndices modIndices = { (int)moduleType, 0 };
  FBParamTopoIndices paramIndices = { modIndices, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  bool on = renderState->AudioParamList<int>(paramIndices, false, -1) != 0;

  paramIndices = { modIndices, { (int)FFEchoParam::Order, 0 } };
  auto order = renderState->AudioParamList<FFEchoOrder>(paramIndices, false, -1);
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;

  int tapsOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Taps);
  FBRenderModuleGraph<Global, true>(renderData, tapsOrder);
  graphData->graphs[tapsOrder].text = moduleName + " Taps";
  if(!on || !IsTapsOn(renderState, Global, false, -1))
    graphData->graphs[tapsOrder].text += " Off";

  int feedbackOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Feedback);
  FBRenderModuleGraph<Global, true>(renderData, feedbackOrder);
  graphData->graphs[feedbackOrder].text = moduleName + " Feedback";
  if (!on || !IsFeedbackOn(renderState, Global, false, -1))
    graphData->graphs[feedbackOrder].text += " Off";

  int reverbOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Reverb);
  FBRenderModuleGraph<Global, true>(renderData, reverbOrder);
  graphData->graphs[reverbOrder].text = moduleName + " Reverb";
  if (!on || !IsReverbOn(renderState, Global, false, -1))
    graphData->graphs[reverbOrder].text += " Off";

  int allOrder = (int)FFEchoModule::Count;
  FBRenderModuleGraph<Global, true>(renderData, allOrder);
  graphData->graphs[allOrder].text = moduleName;
  if (!on)
    graphData->graphs[allOrder].text += " Off";
}

template struct EchoGraphRenderData<true>;
template struct EchoGraphRenderData<false>;
template void FFEchoRenderGraph<true>(FBModuleGraphComponentData*);
template void FFEchoRenderGraph<false>(FBModuleGraphComponentData*);