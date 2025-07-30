#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/env/FFEnvGraph.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct EnvGraphRenderData final:
public FBModuleGraphRenderData<EnvGraphRenderData>
{
  FFEnvProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoPostProcess(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

static void
StageLengthAudioSamples(
  FBGraphRenderState const* state, 
  bool exchange, int exchangeVoice, 
  std::vector<int>& stageLengths, int& smoothLength)
{
  stageLengths = {};
  float bpm = state->ExchangeContainer()->Host()->bpm;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  
  auto type = state->AudioParamList<FFEnvType>(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } }, exchange, exchangeVoice);
  bool sync = state->AudioParamBool(
    { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Sync, 0 } }, exchange, exchangeVoice);
  if (type == FFEnvType::Off)
    return;

  if (!sync)
  {
    for (int i = 0; i < FFEnvStageCount; i++)
      stageLengths.push_back(state->AudioParamLinearTimeSamples(
        { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } }, exchange, exchangeVoice, sampleRate));
    smoothLength = state->AudioParamLinearTimeSamples(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothTime, 0 } }, exchange, exchangeVoice, sampleRate);
  }
  else
  {
    for (int i = 0; i < FFEnvStageCount; i++)
      stageLengths.push_back(state->AudioParamBarsSamples(
        { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } }, exchange, exchangeVoice, sampleRate, bpm));
    smoothLength = state->AudioParamBarsSamples(
      { { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothBars, 0 } }, exchange, exchangeVoice, sampleRate, bpm);
  }
}

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, int /*graphIndex*/)
{
  FBModuleGraphPlotParams result = {};
  result.sampleCount = 0;
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.staticModuleIndex = (int)FFModuleType::Env;

  int smoothLength = 0;
  std::vector<int> stageLengths;
  auto const* state = data->renderState;
  StageLengthAudioSamples(state, false, -1, stageLengths, smoothLength);
  for (int i = 0; i < stageLengths.size(); i++)
    result.sampleCount += stageLengths[i];
  result.sampleCount += smoothLength;
  return result;
}

int 
EnvGraphRenderData::DoProcess(
  FBGraphRenderState* state, int /*graphIndex*/,
  bool /*exchange*/, int /*exchangeVoice*/)
{ 
  auto* moduleProcState = state->ModuleProcState();
  return GetProcessor(*moduleProcState).Process(*moduleProcState);
}

void 
EnvGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int /*graphIndex*/,
  bool /*exchange*/, int /*exchangeVoice*/)
{ 
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).BeginVoice(*moduleProcState);
}

FFEnvProcessor&
EnvGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].env[state.moduleSlot].processor;
}

void
EnvGraphRenderData::DoProcessIndicators(
  int /*graphIndex*/, bool exchange,
  int exchangeVoice, FBModuleGraphPoints& points)
{
  int smoothLengthAudio;
  int totalSamplesAudio = 0;
  std::vector<int> stageLengthsAudio;
  StageLengthAudioSamples(graphData->renderState, exchange, exchangeVoice, stageLengthsAudio, smoothLengthAudio);
  for (int i = 0; i < stageLengthsAudio.size(); i++)
    totalSamplesAudio += stageLengthsAudio[i];
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
}

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  EnvGraphRenderData renderData = {};
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  graphData->drawMarkersSelector = [](int) { return true; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].env[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].env[slot].output; };
  
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  for (int o = 0; o < FFEnvCount; o++)
  {
    graphData->renderState->ModuleProcState()->moduleSlot = o;
    FBRenderModuleGraph<false, false>(renderData, o);
    FBTopoIndices modIndices = { (int)FFModuleType::Env, o };
    FBParamTopoIndices paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFEnvParam::Type, 0 } };
    graphData->graphs[o].text = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
    if (graphData->renderState->AudioParamList<FFEnvType>(paramIndices, false, -1) == FFEnvType::Off)
      graphData->graphs[o].text += " OFF";
  }
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}