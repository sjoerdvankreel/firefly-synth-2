#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/echo/FFEchoGraph.hpp>
#include <firefly_synth/modules/echo/FFEchoProcessor.hpp>

#include <firefly_base/gui/graph/FBGraphing.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
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
class EchoGraphProcessor final:
public FBModuleGraphProcessor
{
public:
  int totalSamples = {};
  std::array<int, (int)FFEchoModule::Count> samplesProcessed = {};

  EchoGraphProcessor(FBModuleGraphComponentData* componentData) : 
  FBModuleGraphProcessor(componentData) {}
  FFEchoProcessor<Global>& GetProcessor(FBModuleProcState& state);

  FBSArray2<float, FBFixedBlockSamples, 2> const* StereoOutput(
    void const* procState, FBModuleGraphStateParams const& params) override;
  FBModuleProcExchangeStateBase const* ExchangeState(
    void const* exchangeState, FBModuleGraphStateParams const& params) override;

  FBModuleGraphPlotParams PlotParams(
    bool detailGraphs, int graphIndex) const override;
  int Process(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void BeginVoiceOrBlock(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void ReleaseOnDemandBuffers(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void ProcessIndicators(FBGraphRenderState*,
    FBModuleGraphProcessParams const&, FBModuleGraphPoints&) override {};
  void PostProcessMarker(FBGraphRenderState* /*state*/,
    FBModuleGraphData& /*data*/, FBModuleGraphProcessParams const& /*params*/, float& /*positionNormalized*/, bool& /*displayMarker*/) override { }
  void PostProcess(FBGraphRenderState* state,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) override;
  void ProcessExchangeState(FBGraphRenderState* graphState,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState) override;
};

template <bool Global>
FFEchoProcessor<Global>&
EchoGraphProcessor<Global>::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *FFSelectDualState<Global>(
    [procState] { return procState->dsp.global.gEcho.processor.get(); },
    [procState, &state] { return procState->dsp.voice[state.voice->slot].vEcho.processor.get(); });
}

template <bool Global>
void
EchoGraphProcessor<Global>::PostProcess(
  FBGraphRenderState* /*state*/, FBModuleGraphData& /*data*/,
  FBModuleGraphProcessParams const& /*params*/, FBModuleGraphPoints& points)
{
  points.bipolar = true;
}

template <bool Global>
FBSArray2<float, FBFixedBlockSamples, 2> const* 
EchoGraphProcessor<Global>::StereoOutput(
  void const* procState, FBModuleGraphStateParams const& params)
{
  if(params.global)
    return &static_cast<FFProcState const*>(procState)->dsp.global.gEcho.output;
  else
    return &static_cast<FFProcState const*>(procState)->dsp.voice[params.voice].vEcho.output;
}

template <bool Global>
FBModuleProcExchangeStateBase const* 
EchoGraphProcessor<Global>::ExchangeState(
  void const* exchangeState, FBModuleGraphStateParams const& params)
{
  if(params.global)
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gEcho[0];
  else
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[params.voice].vEcho[0];
}

template <bool Global>
FBModuleGraphPlotParams
EchoGraphProcessor<Global>::PlotParams(bool /*detailGraphs*/, int /*graphIndex*/) const
{
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = ComponentData()->pixelWidth;
  result.sampleRate = ComponentData()->pixelWidth / FFEchoPlotLengthSeconds;
  result.staticModuleIndex = (int)(Global ? FFModuleType::GEcho : FFModuleType::VEcho);
  return result;
}

template <bool Global>
void
EchoGraphProcessor<Global>::ReleaseOnDemandBuffers(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& /*params*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).ReleaseOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(),
    state->ProcContainer());
}

template <bool Global>
void 
EchoGraphProcessor<Global>::BeginVoiceOrBlock(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& params)
{ 
  samplesProcessed[params.graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).AllocOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(), 
    state->ProcContainer(),
    true, moduleProcState->input->sampleRate);
  GetProcessor(*moduleProcState).BeginVoiceOrBlock(
    *moduleProcState, true, params.detailGraphs, params.graphIndex, totalSamples);
}

template <bool Global>
void
EchoGraphProcessor<Global>::ProcessExchangeState(
  FBGraphRenderState* graphState, FBModuleGraphData& data, 
  FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState)
{
  auto echoExchange = dynamic_cast<FFEchoExchangeState const*>(exchangeState);
  if (!params.detailGraphs)
  {
    data.exchangeGainValue = std::max(data.exchangeGainValue, echoExchange->output);
    data.exchangeMainText = FBToStringPercent(echoExchange->inputGain, 2) + " Gain";
    return;
  }

  auto moduleType = Global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } };
  auto order = graphState->AudioParamList<FFEchoOrder>(indices, false, -1);
  if (params.graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Taps))
  {
    data.exchangeMainText = FBToStringPercent(echoExchange->tapsMix, 2) + " Mix";
    data.exchangeGainValue = std::max(data.exchangeGainValue, echoExchange->outputTaps);
  }
  else if (params.graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Feedback))
  {
    data.exchangeMainText = FBToStringSeconds(echoExchange->feedbackDelay, 3);
    data.exchangeGainValue = std::max(data.exchangeGainValue, echoExchange->outputFeedback);
  }
  else if (params.graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Reverb))
  {
    data.exchangeMainText = FBToStringPercent(echoExchange->reverbSize, 2) + " Size";
    data.exchangeGainValue = std::max(data.exchangeGainValue, echoExchange->outputReverb);
  }
  else
    FB_ASSERT(false);
}

template <bool Global>
int 
EchoGraphProcessor<Global>::Process(
  FBGraphRenderState* state,
  FBModuleGraphProcessParams const& params)
{
  auto* moduleProcState = state->ModuleProcState();
  auto moduleType = Global ? FFModuleType::GEcho : FFModuleType::VEcho;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::VTargetOrGTarget, 0 } };
  auto target = state->AudioParamList<int>(indices, false, -1);
  if (target == 0)
    return 0;

  if (params.detailGraphs)
  {
    indices = { { (int)moduleType, 0 }, { (int)FFEchoParam::Order, 0 } };
    auto order = state->AudioParamList<FFEchoOrder>(indices, false, -1);
    if (params.graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Taps) && 
      !IsTapsOn(state, Global, params.exchange, params.exchangeVoice))
      return 0;
    if (params.graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Feedback) && 
      !IsFeedbackOn(state, Global, params.exchange, params.exchangeVoice))
      return 0;
    if (params.graphIndex == FFEchoGetProcessingOrder(order, FFEchoModule::Reverb) && 
      !IsReverbOn(state, Global, params.exchange, params.exchangeVoice))
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
      int inputSample = samplesProcessed[params.graphIndex] + s;
      float sawIn = FBToBipolar((inputSample - delaySamples) / (float)FBFixedBlockSamples);
      input[c].Set(s, delaySamples <= inputSample && inputSample <= FBFixedBlockSamples + delaySamples ? sawIn : 0.0f);
    }
  
  samplesProcessed[params.graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).Process(*moduleProcState, true, -1);
}

template <bool Global>
void
FFEchoRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  EchoGraphProcessor<Global> processor(graphData);
  graphData->skipDrawOnEqualsPrimary = true;
  processor.totalSamples = processor.PlotParams(Global, -1).sampleCount;

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

  if (!detailGraphs)
  {
    FBRenderModuleGraph(&processor, Global, true, detailGraphs, 0);
    graphData->graphs[0].moduleSlot = 0;
    graphData->graphs[0].moduleIndex = (int)moduleType;
    graphData->graphs[0].displayGainAsDb = true;
    graphData->graphs[0].title = moduleName;
    graphData->graphs[0].title += ": " + (Global ?
      FFGEchoTargetToString((FFGEchoTarget)target) :
      FFVEchoTargetToString((FFVEchoTarget)target));
    graphData->graphs[0].defaultMainText = FBGainToStringDb(gain, 2) + " Gain";
    graphData->graphs[0].MergeStereo();
    graphData->graphs[0].ScaleToSelfNormalized();
    return;
  }

  int tapCount = 0;
  for (int i = 0; i < FFEchoTapCount; i++)
  {
    paramIndices = { modIndices, { (int)FFEchoParam::TapOn, i } };
    tapCount += renderState->AudioParamBool(paramIndices, false, -1) ? 1 : 0;
  }
  paramIndices = { modIndices, { (int)FFEchoParam::TapsMix, 0 } };
  bool tapsOn = IsTapsOn(renderState, Global, false, -1);
  float tapsMix = renderState->AudioParamIdentity(paramIndices, false, -1);
  int tapsOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Taps);
  FBRenderModuleGraph(&processor, Global, true, detailGraphs, tapsOrder);
  graphData->graphs[tapsOrder].moduleSlot = 0;
  graphData->graphs[tapsOrder].moduleIndex = (int)moduleType;
  graphData->graphs[tapsOrder].displayGainAsDb = true;
  graphData->graphs[tapsOrder].title = "Multi Tap: ";
  graphData->graphs[tapsOrder].title += IsTapsOn(renderState, Global, false, -1) ? "On" : "Off";
  if (tapsOn)
    graphData->graphs[tapsOrder].title += ", " + std::to_string(tapCount) + " Taps";
  graphData->graphs[tapsOrder].defaultMainText = FBToStringPercent(tapsMix, 2) + " Mix";
  graphData->graphs[tapsOrder].ScaleToSelfNormalized();

  bool feedbackOn = IsFeedbackOn(renderState, Global, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::Sync, 0 } };
  bool sync = renderState->AudioParamBool(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::FeedbackDelayTime, 0 } };
  float feedbackDelayTime = renderState->AudioParamLinear(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::FeedbackDelayBars, 0 } };
  double feedbackDelayBarsNorm = renderState->AudioParamNormalized(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::FeedbackLPOn, 0 } };
  bool feedbackLPOn = renderState->AudioParamBool(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::FeedbackHPOn, 0 } };
  bool feedbackHPOn = renderState->AudioParamBool(paramIndices, false, -1);
  int feedbackOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Feedback);
  FBRenderModuleGraph(&processor, Global, true, detailGraphs, feedbackOrder);
  graphData->graphs[feedbackOrder].moduleSlot = 0;
  graphData->graphs[feedbackOrder].moduleIndex = (int)moduleType;
  graphData->graphs[feedbackOrder].displayGainAsDb = true;
  graphData->graphs[feedbackOrder].title = "Feedback: ";
  graphData->graphs[feedbackOrder].title += feedbackOn ? "On" : "Off";
  if (feedbackOn && feedbackLPOn)
    graphData->graphs[feedbackOrder].title += ", LPF";
  if (feedbackOn && feedbackHPOn)
    graphData->graphs[feedbackOrder].title += ", HPF";
  graphData->graphs[feedbackOrder].defaultMainText = !sync ?
    FBToStringSeconds(feedbackDelayTime, 3) :
    (moduleTopo.params[(int)FFEchoParam::FeedbackDelayBars].BarsNonRealTime().NormalizedToText(false, 0, feedbackDelayBarsNorm) + " Bars");
  graphData->graphs[feedbackOrder].ScaleToSelfNormalized();

  bool reverbOn = IsReverbOn(renderState, Global, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::ReverbSize, 0 } };
  float reverbSize = renderState->AudioParamIdentity(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::ReverbLPOn, 0 } };
  bool reverbLPOn = renderState->AudioParamBool(paramIndices, false, -1);
  paramIndices = { modIndices, { (int)FFEchoParam::ReverbHPOn, 0 } };
  bool reverbHPOn = renderState->AudioParamBool(paramIndices, false, -1);
  int reverbOrder = FFEchoGetProcessingOrder(order, FFEchoModule::Reverb);
  FBRenderModuleGraph(&processor, Global, true, detailGraphs, reverbOrder);
  graphData->graphs[reverbOrder].moduleSlot = 0;
  graphData->graphs[reverbOrder].moduleIndex = (int)moduleType;
  graphData->graphs[reverbOrder].displayGainAsDb = true;
  graphData->graphs[reverbOrder].title = "Reverb: ";
  graphData->graphs[reverbOrder].title += reverbOn ? "On" : "Off";
  if (reverbOn && reverbLPOn)
    graphData->graphs[reverbOrder].title += ", LPF";
  if (reverbOn && reverbHPOn)
    graphData->graphs[reverbOrder].title += ", HPF";
  graphData->graphs[reverbOrder].defaultMainText = FBToStringPercent(reverbSize, 2) + " Size";
  graphData->graphs[reverbOrder].ScaleToSelfNormalized();
}

template class EchoGraphProcessor<true>;
template class EchoGraphProcessor<false>;
template void FFEchoRenderGraph<true>(FBModuleGraphComponentData*, bool);
template void FFEchoRenderGraph<false>(FBModuleGraphComponentData*, bool);