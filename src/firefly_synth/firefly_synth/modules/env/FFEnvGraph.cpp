#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/env/FFEnvGraph.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

enum class EnvSection
{
  AttackDecay,
  Loop,
  Release,
  All,
};

struct EnvSectionDetails
{
  int stageEnd = {};
  int stageStart = {};
  bool haveSection = {};
  int sectionStartSamples = {};
  int sectionLengthSamples = {};
  std::vector<int> stageLengths = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(EnvSectionDetails);
};

struct EnvDetails
{
  int smoothLength = {};
  EnvSectionDetails all = {};
  EnvSectionDetails loop = {};
  EnvSectionDetails release = {};
  EnvSectionDetails attackDecay = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(EnvDetails);

  EnvSectionDetails const& GetSectionDetails(EnvSection section)
  {
    switch (section)
    {
    case EnvSection::All: return all;
    case EnvSection::Loop: return loop;
    case EnvSection::Release: return release;
    case EnvSection::AttackDecay: return attackDecay;
    default: FB_ASSERT(false); return *((EnvSectionDetails*)(nullptr));
    }
  }
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
    FBModuleGraphData& /*data*/, FBModuleGraphProcessParams const& /*params*/, float& /*positionNormalized*/, bool& displayMarker) override;
  void PostProcess(FBGraphRenderState* state,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) override;
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
  if (type == FFEnvType::Off)
    return;

  bool sync = state->AudioParamBool(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Sync, 0 } }, exchange, exchangeVoice);
  int loopStart = state->AudioParamDiscrete(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } }, exchange, exchangeVoice);
  int loopLength = state->AudioParamDiscrete(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } }, exchange, exchangeVoice);
  int releasePoint = state->AudioParamDiscrete(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } }, exchange, exchangeVoice);
  
  int sustainStart = loopStart == 0 ? -1 : loopStart - 1;
  int sustainEnd = loopStart == 0 ? -1 : loopStart - 1 + loopLength;
  int releaseStart = releasePoint == 0 ? -1 : releasePoint;
  int attackDecayEnd = sustainStart != -1 ? sustainStart : releaseStart != -1 ? releaseStart : -1;

  details.all.haveSection = true;
  details.all.stageStart = 0;
  details.all.stageEnd = FFEnvStageCount;
  if (sync)
    details.smoothLength = state->AudioParamBarsSamples(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothBars, 0 } }, exchange, exchangeVoice, sampleRate, bpm);
  else
    details.smoothLength = state->AudioParamLinearTimeSamples(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothTime, 0 } }, exchange, exchangeVoice, sampleRate);

  int stageLength;
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    if (sync)
      stageLength = state->AudioParamBarsSamples(
        { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } }, exchange, exchangeVoice, sampleRate, bpm);
    else
      stageLength = state->AudioParamLinearTimeSamples(
        { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } }, exchange, exchangeVoice, sampleRate);
    details.all.sectionStartSamples = 0;
    details.all.sectionLengthSamples += stageLength;
    details.all.stageLengths.push_back(stageLength);
    if (releaseStart != -1)
    {
      if (i >= releaseStart)
      {
        details.release.haveSection = true;
        details.release.stageStart = releaseStart;
        details.release.stageEnd = FFEnvStageCount;
        details.release.sectionLengthSamples += stageLength;
        details.release.stageLengths.push_back(stageLength);
      }
      else
      {
        details.release.sectionStartSamples += stageLength;
      }
    }
    if (sustainStart != -1)
    {
      if (i >= sustainStart && i <= sustainEnd)
      {
        details.loop.haveSection = true;
        details.loop.stageEnd = sustainEnd;
        details.loop.stageStart = sustainStart;
        if (i < sustainEnd)
        {
          details.loop.sectionLengthSamples += stageLength;
          details.loop.stageLengths.push_back(stageLength);
        }
      }
      else if (i < sustainStart)
      {
        details.loop.sectionStartSamples += stageLength;
      }
    }
    if (attackDecayEnd != -1 && i < attackDecayEnd)
    {
      details.attackDecay.haveSection = true;
      details.attackDecay.sectionStartSamples = 0;
      details.attackDecay.stageStart = 0;
      details.attackDecay.stageEnd = attackDecayEnd;
      details.attackDecay.sectionLengthSamples += stageLength;
      details.attackDecay.stageLengths.push_back(stageLength);
    }
  }
}

static int 
GetRenderLengthSamples(
  EnvDetails const& details,
  bool detailGraphs, int graphIndex)
{
  // toss out the rest later
  // seems too hard to start in the middle of the env
  if (!detailGraphs)
    return details.all.sectionLengthSamples + details.smoothLength;
  switch ((EnvSection)graphIndex)
  {
  case EnvSection::Release: return details.all.sectionLengthSamples;
  case EnvSection::AttackDecay: return details.attackDecay.sectionLengthSamples;
  case EnvSection::Loop: return details.attackDecay.sectionLengthSamples + details.loop.sectionLengthSamples;
  default: FB_ASSERT(false); return -1;
  }
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
  return GetProcessor(*moduleProcState).Process(*moduleProcState, exchangeFromDSP, true, !params.detailGraphs, -1);
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
  int graphSampleCount = GetRenderLengthSamples(details, params.detailGraphs, params.graphIndex);

  auto* moduleProcState = state->ModuleProcState();
  auto const* exchangeFromDSP = GetEnvExchangeState(state, params.exchange, params.exchangeVoice);
  GetProcessor(*moduleProcState).BeginVoice(*moduleProcState, exchangeFromDSP, true, graphSampleCount);
}

void 
EnvGraphProcessor::ProcessExchangeState(
  FBGraphRenderState* graphState, FBModuleGraphData& data, 
  FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState)
{
  auto envExchange = dynamic_cast<FFEnvExchangeState const*>(exchangeState);
  if (!params.detailGraphs)
  {
    // TODO data.exchangeMainText = FBToStringHz(lfoExchange->rates[0], 2);
    data.exchangeGainValue = std::max(data.exchangeGainValue, envExchange->output);
    return;
  }

  // TODO data.exchangeMainText = FBToStringHz(lfoExchange->rates[params.graphIndex], 2);
  EnvDetails details = {};
  GetEnvelopeDetails(graphState, params.exchange, params.exchangeVoice, details);
  if (params.graphIndex == (int)EnvSection::AttackDecay)
    data.exchangeGainValue = std::max(data.exchangeGainValue, envExchange->outputAttack);
  else if (params.graphIndex == (int)EnvSection::Loop)
    data.exchangeGainValue = std::max(data.exchangeGainValue, envExchange->outputLoop);
  else if (params.graphIndex == (int)EnvSection::Release)
    data.exchangeGainValue = std::max(data.exchangeGainValue, envExchange->outputRelease);
  else
    FB_ASSERT(false);
}

FBModuleGraphPlotParams
EnvGraphProcessor::PlotParams(
  bool detailGraphs, int graphIndex) const
{
  FBModuleGraphPlotParams result = {};
  result.sampleCount = 0;
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.staticModuleIndex = (int)FFModuleType::Env;

  EnvDetails details = {};
  GetEnvelopeDetails(ComponentData()->renderState, false, -1, details);
  result.sampleCount = GetRenderLengthSamples(details, detailGraphs, graphIndex);
  return result;
}

void 
EnvGraphProcessor::PostProcessMarker(
  FBGraphRenderState* state, FBModuleGraphData& /*data*/,
  FBModuleGraphProcessParams const& params, float& positionNormalized, bool& displayMarker)
{
  if (!params.detailGraphs)
    return;

  EnvDetails details = {};
  GetEnvelopeDetails(state, params.exchange, params.exchangeVoice, details);
  auto const& sectionDetails = details.GetSectionDetails(!params.detailGraphs ? EnvSection::All : (EnvSection)params.graphIndex);
  if (!sectionDetails.haveSection || sectionDetails.sectionLengthSamples == 0)
  {
    positionNormalized = 0.0f;
    return;
  }

  positionNormalized -= sectionDetails.sectionStartSamples / (float)details.all.sectionLengthSamples;
  displayMarker &= positionNormalized >= 0.0f; // still in prev
  positionNormalized *= details.all.sectionLengthSamples / (float)sectionDetails.sectionLengthSamples;
  displayMarker &= positionNormalized < 0.99f; // next one takes over
  positionNormalized = std::clamp(positionNormalized, 0.0f, 1.0f);
}

void 
EnvGraphProcessor::PostProcess(
  FBGraphRenderState* state, FBModuleGraphData& /*data*/,
  FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points)
{
  if (!params.detailGraphs)
    return;

  EnvDetails details = {};
  GetEnvelopeDetails(state, params.exchange, params.exchangeVoice, details);
  auto const& sectionDetails = details.GetSectionDetails(!params.detailGraphs? EnvSection::All: (EnvSection)params.graphIndex);
  if (!sectionDetails.haveSection)
  {
    points = {};
    return;
  }

  // details are against dsp sample rate,
  // series is against gui sample rate
  int start = (int)(sectionDetails.sectionStartSamples * params.guiSampleRate / params.hostSampleRate);
  int length = (int)(sectionDetails.sectionLengthSamples * params.guiSampleRate / params.hostSampleRate);
  points.l.erase(points.l.begin(), points.l.begin() + std::min(start, (int)points.l.size()));
  points.l.erase(points.l.begin() + std::min(length, (int)points.l.size()), points.l.end());
}

void
EnvGraphProcessor::ProcessIndicators(
  FBGraphRenderState* state,
  FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points)
{
  if (params.detailGraphs)
    return;

  int totalSamplesAudio = 0;
  std::vector<int> stageLengthsAudio;
  auto const* exchangeFromDSP = GetEnvExchangeState(state, params.exchange, params.exchangeVoice);

  EnvDetails details = {};
  GetEnvelopeDetails(state, params.exchange, params.exchangeVoice, details);
  stageLengthsAudio = details.all.stageLengths;
  for (int i = 0; i < stageLengthsAudio.size(); i++)
  {
    if (exchangeFromDSP != nullptr &&
      (exchangeFromDSP->boolThisVoiceIsSubSectionStart != 0) &&
      i == 0 &&
      state->ModuleProcState()->moduleSlot == FFAmpEnvSlot)
      stageLengthsAudio[i] = (int)std::round((float)stageLengthsAudio[i] * exchangeFromDSP->portaSectionAmpAttack);
    totalSamplesAudio += stageLengthsAudio[i];
  }
  totalSamplesAudio += details.smoothLength;
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
  int graphCount = detailGraphs ? 3 : 1;
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  for (int i = 0; i < graphCount; i++)
  {
    FBRenderModuleGraph(&processor, false, false, detailGraphs, i);
    FBTopoIndices modIndices = { (int)FFModuleType::Env, moduleSlot };
    graphData->graphs[i].moduleSlot = moduleSlot;
    graphData->graphs[i].moduleIndex = (int)FFModuleType::Env;

    float maxVal = 0.0f;
    for (int j = 0; j < graphData->graphs[i].primarySeries.l.size(); j++)
      maxVal = std::max(maxVal, graphData->graphs[i].primarySeries.l[j]);
    graphData->graphs[i].displayGainAsDb = false;
    graphData->graphs[i].hasDefaultGainValue = true;
    graphData->graphs[i].defaultGainValue = maxVal;

    if (!detailGraphs)
    {
      paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFEnvParam::Type, 0 } };
      auto type = graphData->renderState->AudioParamList<FFEnvType>(paramIndices, false, -1);
      graphData->graphs[i].title = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
      graphData->graphs[i].title += ": " + FFEnvTypeToString(type);
      if (type != FFEnvType::Off)
      {
        paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFEnvParam::Sync, 0 } };
        bool sync = graphData->renderState->AudioParamBool(paramIndices, false, -1);
        graphData->graphs[i].title += std::string(", ") + (sync ? "BPM" : "Time");
      }
    }
    else
    {
      auto const& sectionDetails = details.GetSectionDetails((EnvSection)i);
      if (i == (int)EnvSection::AttackDecay)
        graphData->graphs[i].title = "Attack/Decay";
      else if (i == (int)EnvSection::Loop)
        graphData->graphs[i].title = "Loop/Sustain";
      else if (i == (int)EnvSection::Release)
        graphData->graphs[i].title = "Release";
      else
        FB_ASSERT(false);
      graphData->graphs[i].title += sectionDetails.haveSection ? ": On" : ": Off";
    }
  }
}