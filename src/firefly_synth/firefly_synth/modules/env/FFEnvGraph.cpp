#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/env/FFEnvGraph.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

enum class EnvSection
{
  All,
  AttackDecay,
  Loop,
  Release
};

struct EnvSectionDetails
{
  int stageEnd = {};
  int stageStart = {};
  bool haveSection = {};
  int sectionLengthTotal = {};
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

struct EnvGraphRenderData final:
public FBModuleGraphRenderData<EnvGraphRenderData>
{
  FFEnvProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/) {}
  void DoPostProcess(FBGraphRenderState* /*state*/, FBModuleGraphData& /*data*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoProcessExchangeState(FBGraphRenderState* /*graphState*/, FBModuleGraphData& /*data*/, bool /*detailGraphs*/, int /*graphIndex*/, int /*exchangeVoice*/, FBModuleProcExchangeStateBase const* /*exchangeState*/) {}
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
  int releaseStart = releasePoint == 0 ? -1 : releasePoint - 1;
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
    details.all.sectionLengthTotal += stageLength;
    details.all.stageLengths.push_back(stageLength);
    if (releaseStart != -1 && i >= releaseStart)
    {
      details.release.haveSection = true;
      details.release.stageStart = releaseStart;
      details.release.stageEnd = FFEnvStageCount;
      details.release.sectionLengthTotal += stageLength;
      details.release.stageLengths.push_back(stageLength);
    }
    if (sustainStart != -1 && i >= sustainStart && i <= sustainEnd)
    {
      details.loop.haveSection = true;
      details.loop.stageEnd = sustainEnd;
      details.loop.stageStart = sustainStart;
      if (i < sustainEnd)
      {
        details.loop.sectionLengthTotal += stageLength;
        details.loop.stageLengths.push_back(stageLength);
      }
    }
    if (attackDecayEnd != -1 && i < attackDecayEnd)
    {
      details.attackDecay.haveSection = true;
      details.attackDecay.stageStart = 0;
      details.attackDecay.stageEnd = attackDecayEnd;
      details.attackDecay.sectionLengthTotal += stageLength;
      details.attackDecay.stageLengths.push_back(stageLength);
    }
  }
}

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, bool detailGraphs, int /*graphIndex*/)
{
  FBModuleGraphPlotParams result = {};
  result.sampleCount = 0;
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.staticModuleIndex = (int)FFModuleType::Env;

  EnvDetails details = {};
  GetEnvelopeDetails(data->renderState, false, -1, details);
  auto const& sectionDetails = details.GetSectionDetails(EnvSection::All);
  result.sampleCount = sectionDetails.sectionLengthTotal;
  if (!detailGraphs)
    result.sampleCount += details.smoothLength;
  return result;
}

FFEnvProcessor&
EnvGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].env[state.moduleSlot].processor;
}

void
EnvGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, 
  bool /*detailGraphs*/, int /*graphIndex*/,
  bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  auto const* exchangeFromDSP = GetEnvExchangeState(state, exchange, exchangeVoice);
  GetProcessor(*moduleProcState).BeginVoice(*moduleProcState, exchangeFromDSP, true);
}

int 
EnvGraphRenderData::DoProcess(
  FBGraphRenderState* state, 
  bool detailGraphs, int /*graphIndex*/,
  bool exchange, int exchangeVoice)
{ 
  auto* moduleProcState = state->ModuleProcState();
  auto const* exchangeFromDSP = GetEnvExchangeState(state, exchange, exchangeVoice);
  return GetProcessor(*moduleProcState).Process(*moduleProcState, exchangeFromDSP, true, !detailGraphs, -1);
}

void
EnvGraphRenderData::DoProcessIndicators(
  FBGraphRenderState* /*state*/,
  bool /*detailGraphs*/, int /*graphIndex*/, 
  bool /*/exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/)
{
#if 0 // todo
  int smoothLengthAudio;
  int totalSamplesAudio = 0;
  std::vector<int> stageLengthsAudio;
  auto const* exchangeFromDSP = GetEnvExchangeState(state, exchange, exchangeVoice);

  StageLengthAudioSamples(graphData->renderState, exchange, exchangeVoice, stageLengthsAudio, smoothLengthAudio);
  for (int i = 0; i < stageLengthsAudio.size(); i++)
  {
    if (exchangeFromDSP != nullptr &&
      (exchangeFromDSP->boolThisVoiceIsSubSectionStart != 0) &&
      i == 0 &&
      state->ModuleProcState()->moduleSlot == FFAmpEnvSlot)
      stageLengthsAudio[i] = (int)std::round((float)stageLengthsAudio[i] * exchangeFromDSP->portaSectionAmpAttack);
    totalSamplesAudio += stageLengthsAudio[i];
  }
  totalSamplesAudio += smoothLengthAudio;
  float thisSamplesGUI = static_cast<float>(points.l.size());

  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  auto type = graphData->renderState->AudioParamList<FFEnvType>(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } }, exchange, exchangeVoice);
  if (type == FFEnvType::Off)
    return;

  int releasePoint = graphData->renderState->AudioParamDiscrete(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } }, exchange, exchangeVoice);
  if (releasePoint != 0)
  {
    int releasePointSamples = 0;
    for (int i = 0; i < releasePoint; i++)
      releasePointSamples += stageLengthsAudio[i];
    releasePointSamples = static_cast<int>(releasePointSamples * thisSamplesGUI / totalSamplesAudio);
    points.pointIndicators.push_back(releasePointSamples);
  }

  int loopStart = graphData->renderState->AudioParamDiscrete(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } }, exchange, exchangeVoice);
  if (loopStart != 0)
  {
    int lp = 0;
    int loopStartSamples = 0;
    for (; lp < loopStart - 1; lp++)
      loopStartSamples += stageLengthsAudio[lp];
    loopStartSamples = static_cast<int>(loopStartSamples * thisSamplesGUI / totalSamplesAudio);
    points.verticalIndicators.push_back(loopStartSamples);

    int loopLength = graphData->renderState->AudioParamDiscrete(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } }, exchange, exchangeVoice);
    if (loopLength != 0 && lp < FFEnvStageCount)
    {
      int loopLengthSamples = 0;
      for (; lp < loopStart - 1 + loopLength && lp < FFEnvStageCount; lp++)
        loopLengthSamples += stageLengthsAudio[lp];
      loopLengthSamples = static_cast<int>(loopLengthSamples * thisSamplesGUI / totalSamplesAudio);
      points.verticalIndicators.push_back(loopStartSamples + loopLengthSamples);
    }
  }
#endif
}

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  EnvGraphRenderData renderData = {};
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  graphData->skipDrawOnEqualsPrimary = false; // porta subsections
  graphData->drawMarkersSelector = [](int) { return true; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].env[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].env[slot].output; };

  int graphCount = detailGraphs ? 3 : 1;
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  for (int i = 0; i < graphCount; i++)
  {
    FBRenderModuleGraph<false, false>(renderData, detailGraphs, i);
    FBTopoIndices modIndices = { (int)FFModuleType::Env, moduleSlot };
    FBParamTopoIndices paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFEnvParam::Type, 0 } };
    graphData->graphs[i].title = FBAsciiToUpper(graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name);
    graphData->graphs[i].moduleSlot = moduleSlot;
    graphData->graphs[i].moduleIndex = (int)FFModuleType::Env;
  }
}