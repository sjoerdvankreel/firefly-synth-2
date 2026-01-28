#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/echo/FFEchoGraph.hpp>
#include <firefly_synth/modules/echo/FFEchoProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

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
  std::array<int, (int)FFEchoModule::Count> samplesProcessed = {};

  FFEchoProcessor<Global>& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoPostProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoProcessIndicators(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoProcessExchangeState(FBGraphRenderState* /*graphState*/, FBModuleGraphData& /*data*/, bool /*detailGraphs*/, int /*graphIndex*/, int /*exchangeVoice*/, FBModuleProcExchangeStateBase const* /*exchangeState*/);
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
EchoGraphRenderData<Global>::DoProcessExchangeState(
  FBGraphRenderState* graphState, FBModuleGraphData& data,
  bool detailGraphs, int graphIndex, int /*exchangeVoice*/,
  FBModuleProcExchangeStateBase const* exchangeState)
{
  auto echoExchange = dynamic_cast<FFEchoExchangeState const*>(exchangeState);
  if (!detailGraphs)
  {
    data.exchangeMainText = FBGainToStringDb(echoExchange->inputGain, 2) + " Gain";
    return;
  }

  auto moduleType = Global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } };
  auto order = graphState->AudioParamList<FFEchoOrder>(indices, false, -1);
  if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Taps))
    data.exchangeMainText = FBToStringPercent(echoExchange->tapsMix, 2) + " Mix";
  else if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Feedback))
    data.exchangeMainText = FBToStringSeconds(echoExchange->feedbackDelay, 3);
  else if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Reverb))
    data.exchangeMainText = FBToStringPercent(echoExchange->reverbSize, 2) + " Size";
  else
    FB_ASSERT(false);
}

template <bool Global>
void
EchoGraphRenderData<Global>::DoReleaseOnDemandBuffers(
  FBGraphRenderState* state, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).ReleaseOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(),
    state->ProcContainer());
}

template <bool Global>
void 
EchoGraphRenderData<Global>::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).AllocOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(), 
    state->ProcContainer(),
    true, moduleProcState->input->sampleRate);
  GetProcessor(*moduleProcState).BeginVoiceOrBlock(
    *moduleProcState, true, detailGraphs, graphIndex, totalSamples);
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
  FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  auto moduleType = Global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  auto target = state->AudioParamList<int>(indices, false, -1);
  if (target == 0)
    return 0;

  if (detailGraphs)
  {
    indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } };
    auto order = state->AudioParamList<FFEchoOrder>(indices, false, -1);
    if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Taps) && !IsTapsOn(state, Global, exchange, exchangeVoice))
      return 0;
    if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Feedback) && !IsFeedbackOn(state, Global, exchange, exchangeVoice))
      return 0;
    if (graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Reverb) && !IsReverbOn(state, Global, exchange, exchangeVoice))
      return 0;
  }

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
  return GetProcessor(*moduleProcState).Process(*moduleProcState, true, -1);
}

template <bool Global>
void
EchoGraphRenderData<Global>::DoPostProcess(
  FBGraphRenderState* /*state*/,
  bool /*detailGraphs*/, int /*graphIndex*/,
  bool /*exchange*/, int /*exchangeVoice*/,
  FBModuleGraphPoints& points)
{
  points.bipolar = true;
}

template <bool Global>
void
FFEchoRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  EchoGraphRenderData<Global> renderData = {};
  graphData->skipDrawOnEqualsPrimary = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = [](auto graphData, bool, int graphIndex) { return PlotParams(graphData, Global, graphIndex); };
  renderData.totalSamples = PlotParams(graphData, Global, -1).sampleCount;
  renderData.globalExchangeSelector = [](void const* exchangeState, int /*slot*/, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gEcho[0]; };
  renderData.globalStereoOutputSelector = [](void const* procState, int /*slot*/, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gEcho.output; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int /*slot*/, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].vEcho[0]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int /*slot*/, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].vEcho.output; };

  auto* renderState = graphData->renderState;
  auto moduleType = Global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBTopoIndices modIndices = { (int)moduleType, 0 };
  auto const* topo = graphData->renderState->PlugGUI()->HostContext()->Topo();
  auto const& moduleTopo = topo->static_->modules[(int)moduleType];
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;

  FBParamTopoIndices paramIndices = { modIndices, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  auto target = renderState->AudioParamList<int>(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::Order, 0 } };
  auto order = renderState->AudioParamList<FFEchoOrder>(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::Gain, 0 } };
  float gain = renderState->AudioParamLinear(paramIndices, false, -1);

  int maxSizeAllSeries = 0;
  float absMaxValueAllSeries = 0.0f;
  if (!detailGraphs)
  {
    FBRenderModuleGraph<Global, true>(renderData, detailGraphs, 0);
    graphData->graphs[0].GetLimits(false, maxSizeAllSeries, absMaxValueAllSeries);

    graphData->graphs[0].moduleSlot = 0;
    graphData->graphs[0].moduleIndex = (int)moduleType;
    graphData->graphs[0].title = moduleName;
    graphData->graphs[0].title += ": " + (Global ?
      FFGEchoTargetToString((FFGEchoTarget)target) :
      FFVEchoTargetToString((FFVEchoTarget)target));
    graphData->graphs[0].defaultMainText = FBGainToStringDb(gain, 2) + " Gain";
    graphData->graphs[0].exchangeSubText = FBGainToStringDb(absMaxValueAllSeries, 2);
    graphData->graphs[0].MergeStereo();
    graphData->graphs[0].ScaleToSelfNormalized();
    return;
  }

  paramIndices = { modIndices, { (int)FFEchoParam::TapsMix, 0 } };
  float tapsMix = renderState->AudioParamIdentity(paramIndices, false, -1);
  int tapsOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Taps);
  FBRenderModuleGraph<Global, true>(renderData, detailGraphs, tapsOrder);
  graphData->graphs[tapsOrder].GetLimits(false, maxSizeAllSeries, absMaxValueAllSeries);
  graphData->graphs[tapsOrder].moduleSlot = 0;
  graphData->graphs[tapsOrder].moduleIndex = (int)moduleType;
  graphData->graphs[tapsOrder].title = moduleName + " Multi Tap: ";
  graphData->graphs[tapsOrder].title += IsTapsOn(renderState, Global, false, -1) ? "On" : "Off";
  graphData->graphs[tapsOrder].defaultMainText = FBToStringPercent(tapsMix, 2) + " Mix";
  graphData->graphs[tapsOrder].exchangeSubText = FBGainToStringDb(absMaxValueAllSeries, 2);
  graphData->graphs[tapsOrder].ScaleToSelfNormalized();

  paramIndices = { modIndices, { (int)FFEchoParam::Sync, 0 } };
  bool sync = renderState->AudioParamBool(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::FeedbackDelayTime, 0 } };
  float feedbackDelayTime = renderState->AudioParamLinear(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::FeedbackDelayBars, 0 } };
  double feedbackDelayBarsNorm = renderState->AudioParamNormalized(paramIndices, false, -1);
  int feedbackOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Feedback);
  FBRenderModuleGraph<Global, true>(renderData, detailGraphs, feedbackOrder);
  graphData->graphs[feedbackOrder].GetLimits(false, maxSizeAllSeries, absMaxValueAllSeries);
  graphData->graphs[feedbackOrder].moduleSlot = 0;
  graphData->graphs[feedbackOrder].moduleIndex = (int)moduleType;
  graphData->graphs[feedbackOrder].title = moduleName + " Feedback: ";
  graphData->graphs[feedbackOrder].title += IsFeedbackOn(renderState, Global, false, -1) ? "On" : "Off";
  graphData->graphs[feedbackOrder].defaultMainText = !sync ?
    FBToStringSeconds(feedbackDelayTime, 3) :
    moduleTopo.params[(int)FFEchoParam::FeedbackDelayBars].BarsNonRealTime().NormalizedToText(false, 0, feedbackDelayBarsNorm);
  graphData->graphs[feedbackOrder].exchangeSubText = FBGainToStringDb(absMaxValueAllSeries, 2);
  graphData->graphs[feedbackOrder].ScaleToSelfNormalized();

  paramIndices = { modIndices, { (int)FFEchoParam::ReverbSize, 0 } };
  float reverbSize = renderState->AudioParamIdentity(paramIndices, false, -1);
  int reverbOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Reverb);
  FBRenderModuleGraph<Global, true>(renderData, detailGraphs, reverbOrder);
  graphData->graphs[reverbOrder].GetLimits(false, maxSizeAllSeries, absMaxValueAllSeries);
  graphData->graphs[reverbOrder].moduleSlot = 0;
  graphData->graphs[reverbOrder].moduleIndex = (int)moduleType;
  graphData->graphs[reverbOrder].title = moduleName + " Reverb: ";
  graphData->graphs[reverbOrder].title += IsReverbOn(renderState, Global, false, -1) ? "On" : "Off";
  graphData->graphs[reverbOrder].defaultMainText = FBToStringPercent(reverbSize, 2) + " Size";
  graphData->graphs[reverbOrder].exchangeSubText = FBGainToStringDb(absMaxValueAllSeries, 2);
  graphData->graphs[reverbOrder].ScaleToSelfNormalized();
}

template struct EchoGraphRenderData<true>;
template struct EchoGraphRenderData<false>;
template void FFEchoRenderGraph<true>(FBModuleGraphComponentData*, bool);
template void FFEchoRenderGraph<false>(FBModuleGraphComponentData*, bool);