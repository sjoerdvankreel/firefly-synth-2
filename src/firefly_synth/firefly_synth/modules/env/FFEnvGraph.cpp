#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/env/FFEnvGraph.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>

#include <firefly_base/gui/graph/FBGraphing.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct EnvDetails
{
  int smoothLengthSamples = {};
  int stagesLengthSamples = {};
  float smoothLengthSeconds = {};
  float stagesLengthSeconds = {};
  std::vector<int> stageLengths = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(EnvDetails);
};

class EnvGraphProcessor final:
public FBModuleGraphProcessor
{
public:
  FFEnvProcessor& GetProcessor(FBModuleProcState& state);
  EnvGraphProcessor(FBModuleGraphComponentData* componentData) :
  FBModuleGraphProcessor(componentData) {}

  FBModuleGraphPlotParams PlotParams(
    bool detailGraphs, int graphIndex) const override;
  FBSArray<float, FBFixedBlockSamples> const* MonoOutput(
    void const* procState, FBModuleGraphStateParams const& params);
  FBModuleProcExchangeStateBase const* ExchangeState(
    void const* exchangeState, FBModuleGraphStateParams const& params);

  int Process(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void BeginVoiceOrBlock(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void ReleaseOnDemandBuffers(FBGraphRenderState* /*state*/,
    FBModuleGraphProcessParams const& /*params*/) override {}
  void ProcessIndicators(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) override;
  void PostProcessMarker(FBGraphRenderState* /*state*/,
    FBModuleGraphData& /*data*/, FBModuleGraphProcessParams const& /*params*/, float& /*positionNormalized*/, bool& /*displayMarker*/) override { }
  void PostProcess(FBGraphRenderState* /*state*/,
    FBModuleGraphData& /*data*/, FBModuleGraphProcessParams const& /*params*/, FBModuleGraphPoints& /*points*/) override { }
  void ProcessExchangeState(FBGraphRenderState* graphState,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState) override;
};

static FFEnvExchangeState const*
GetEnvExchangeState(
FBGraphRenderState* state, bool exchange, int exchangeVoice)
{
  if (!exchange)
    return nullptr;

  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  int staticModuleIndex = (int)FFModuleType::Env;
  int runtimeModuleIndex = moduleProcState->topo->moduleTopoToRuntime.at({ staticModuleIndex, moduleSlot });
  auto const* moduleExchangeState = state->ExchangeContainer()->Modules()[runtimeModuleIndex].get();
  return &dynamic_cast<FFEnvExchangeState const&>(*moduleExchangeState->Voice()[exchangeVoice]);
}

static void
GetEnvelopeDetails(
  FBGraphRenderState const* state, 
  bool exchange, int exchangeVoice, 
  EnvDetails& details)
{
  details = EnvDetails({});
  float bpm = state->ExchangeContainer()->Host()->bpm;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  
  auto type = state->AudioParamList<FFEnvType>(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } }, exchange, exchangeVoice);
  bool sync = state->AudioParamBool(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Sync, 0 } }, exchange, exchangeVoice);
  if (type == FFEnvType::Off)
    return;

  if (sync)
    details.smoothLengthSamples = state->AudioParamBarsSamples(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothBars, 0 } }, exchange, exchangeVoice, sampleRate, bpm);
  else
    details.smoothLengthSamples = state->AudioParamLinearTimeSamples(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothTime, 0 } }, exchange, exchangeVoice, sampleRate);
  details.smoothLengthSeconds = details.smoothLengthSamples / sampleRate;

  int stageLength;
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    if (sync)
      stageLength = state->AudioParamBarsSamples(
        { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } }, exchange, exchangeVoice, sampleRate, bpm);
    else
      stageLength = state->AudioParamLinearTimeSamples(
        { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } }, exchange, exchangeVoice, sampleRate);
    details.stagesLengthSamples += stageLength;
    details.stageLengths.push_back(stageLength);
  }

  details.stagesLengthSeconds = details.stagesLengthSamples / sampleRate;
}

FFEnvProcessor&
EnvGraphProcessor::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].env[state.moduleSlot].processor;
}

int
EnvGraphProcessor::Process(
  FBGraphRenderState* state,
  FBModuleGraphProcessParams const& params)
{
  auto* moduleProcState = state->ModuleProcState();
  auto const* exchangeFromDSP = GetEnvExchangeState(state, params.exchange, params.exchangeVoice);
  return GetProcessor(*moduleProcState).Process(*moduleProcState, exchangeFromDSP, true, -1);
}

FBSArray<float, FBFixedBlockSamples> const*
EnvGraphProcessor::MonoOutput(
  void const* procState, FBModuleGraphStateParams const& params)
{
  return &static_cast<FFProcState const*>(procState)->dsp.voice[params.voice].env[params.moduleSlot].output;
}

FBModuleProcExchangeStateBase const* 
EnvGraphProcessor::ExchangeState(
  void const* exchangeState, FBModuleGraphStateParams const& params)
{
  return &static_cast<FFExchangeState const*>(exchangeState)->voice[params.voice].env[params.moduleSlot];
}

void
EnvGraphProcessor::BeginVoiceOrBlock(
  FBGraphRenderState* state,
  FBModuleGraphProcessParams const& params)
{
  EnvDetails details = {};
  GetEnvelopeDetails(state, params.exchange, params.exchangeVoice, details);
  int graphSampleCount = details.stagesLengthSamples + details.smoothLengthSamples;
  auto* moduleProcState = state->ModuleProcState();
  auto const* exchangeFromDSP = GetEnvExchangeState(state, params.exchange, params.exchangeVoice);
  GetProcessor(*moduleProcState).BeginVoice(*moduleProcState, exchangeFromDSP, true, !params.detailGraphs, graphSampleCount);
}

void 
EnvGraphProcessor::ProcessExchangeState(
  FBGraphRenderState* graphState, FBModuleGraphData& data, 
  FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState)
{
  EnvDetails details = {};
  auto envExchange = dynamic_cast<FFEnvExchangeState const*>(exchangeState);
  GetEnvelopeDetails(graphState, params.exchange, params.exchangeVoice, details);
  data.exchangeMainText = FBToStringSeconds(details.stagesLengthSeconds + details.smoothLengthSeconds, 3);
  data.exchangeGainValue = std::max(data.exchangeGainValue, envExchange->output);
}

FBModuleGraphPlotParams
EnvGraphProcessor::PlotParams(
  bool /*detailGraphs*/, int /*graphIndex*/) const
{
  FBModuleGraphPlotParams result = {};
  result.sampleCount = 0;
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.staticModuleIndex = (int)FFModuleType::Env;

  EnvDetails details = {};
  GetEnvelopeDetails(ComponentData()->renderState, false, -1, details);
  result.sampleCount = details.stagesLengthSamples + details.smoothLengthSamples;
  return result;
}

void
EnvGraphProcessor::ProcessIndicators(
  FBGraphRenderState* state,
  FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points)
{
  int totalSamplesAudio = 0;
  std::vector<int> stageLengthsAudio;
  auto const* exchangeFromDSP = GetEnvExchangeState(state, params.exchange, params.exchangeVoice);

  EnvDetails details = {};
  GetEnvelopeDetails(state, params.exchange, params.exchangeVoice, details);
  stageLengthsAudio = details.stageLengths;
  for (int i = 0; i < stageLengthsAudio.size(); i++)
  {
    if (exchangeFromDSP != nullptr &&
      (exchangeFromDSP->boolThisVoiceIsSubSectionStart != 0) &&
      i == 0 &&
      state->ModuleProcState()->moduleSlot == FFAmpEnvSlot)
      stageLengthsAudio[i] = (int)std::round((float)stageLengthsAudio[i] * exchangeFromDSP->portaSectionAmpAttack);
    totalSamplesAudio += stageLengthsAudio[i];
  }
  totalSamplesAudio += details.smoothLengthSamples;
  float thisSamplesGUI = static_cast<float>(points.l.size());

  int moduleSlot = ComponentData()->renderState->ModuleProcState()->moduleSlot;
  auto type = ComponentData()->renderState->AudioParamList<FFEnvType>(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } }, params.exchange, params.exchangeVoice);
  if (type == FFEnvType::Off)
    return;

  int releasePoint = ComponentData()->renderState->AudioParamDiscrete(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } }, params.exchange, params.exchangeVoice);
  if (releasePoint != 0)
  {
    int releasePointSamples = 0;
    for (int i = 0; i < releasePoint; i++)
      releasePointSamples += stageLengthsAudio[i];
    releasePointSamples = static_cast<int>(releasePointSamples * thisSamplesGUI / totalSamplesAudio);
    points.pointIndicators.push_back(releasePointSamples);
  }

  int loopStart = ComponentData()->renderState->AudioParamDiscrete(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } }, params.exchange, params.exchangeVoice);
  if (loopStart != 0)
  {
    int lp = 0;
    int loopStartSamples = 0;
    for (; lp < loopStart - 1; lp++)
      loopStartSamples += stageLengthsAudio[lp];
    loopStartSamples = static_cast<int>(loopStartSamples * thisSamplesGUI / totalSamplesAudio);
    points.verticalIndicators.push_back(loopStartSamples);

    int loopLength = ComponentData()->renderState->AudioParamDiscrete(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } }, params.exchange, params.exchangeVoice);
    if (loopLength != 0 && lp < FFEnvStageCount)
    {
      int loopLengthSamples = 0;
      for (; lp < loopStart - 1 + loopLength && lp < FFEnvStageCount; lp++)
        loopLengthSamples += stageLengthsAudio[lp];
      loopLengthSamples = static_cast<int>(loopLengthSamples * thisSamplesGUI / totalSamplesAudio);
      points.verticalIndicators.push_back(loopStartSamples + loopLengthSamples);
    }
  }
}

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  EnvGraphProcessor processor(graphData);
  graphData->skipDrawOnEqualsPrimary = false; // porta subsections
  graphData->drawMarkersSelector = [](int) { return true; };

  EnvDetails details = {};
  GetEnvelopeDetails(graphData->renderState, false, -1, details);

  FBParamTopoIndices paramIndices;
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;

  FBRenderModuleGraph(&processor, false, false, detailGraphs, 0);
  FBTopoIndices modIndices = { (int)FFModuleType::Env, moduleSlot };
  graphData->graphs[0].moduleSlot = moduleSlot;
  graphData->graphs[0].moduleIndex = (int)FFModuleType::Env;

  float maxVal = 0.0f;
  for (int j = 0; j < graphData->graphs[0].primarySeries.l.size(); j++)
    maxVal = std::max(maxVal, graphData->graphs[0].primarySeries.l[j]);
  graphData->graphs[0].displayGainAsDb = false;
  graphData->graphs[0].hasDefaultGainValue = true;
  graphData->graphs[0].defaultGainValue = maxVal;

  paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFEnvParam::Type, 0 } };
  auto type = graphData->renderState->AudioParamList<FFEnvType>(paramIndices, false, -1);
  graphData->graphs[0].title = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
  graphData->graphs[0].title += ": " + FFEnvTypeToString(type);
  graphData->graphs[0].defaultMainText = FBToStringSeconds(details.stagesLengthSeconds + details.smoothLengthSeconds, 3);
  if (type != FFEnvType::Off)
  {
    paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFEnvParam::Sync, 0 } };
    bool sync = graphData->renderState->AudioParamBool(paramIndices, false, -1);
    graphData->graphs[0].title += std::string(", ") + (sync ? "BPM" : "Time");
  }
}