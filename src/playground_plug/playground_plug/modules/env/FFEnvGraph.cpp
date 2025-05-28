#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct EnvGraphRenderData final:
public FBModuleGraphRenderData<EnvGraphRenderData>
{
  FFEnvProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBModuleProcState& state) { return GetProcessor(state).Process(state); }
  void DoBeginVoice(FBModuleProcState& state) { GetProcessor(state).BeginVoice(state); }
  void DoProcessIndicators(bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
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
  bool sync = state->AudioParamBool(
    { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sync, 0 }, exchange, exchangeVoice);

  if (!sync)
  {
    for (int i = 0; i < FFEnvStageCount; i++)
      stageLengths.push_back(state->AudioParamLinearTimeSamples(
        { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageTime, i }, exchange, exchangeVoice, sampleRate));
    smoothLength = state->AudioParamLinearTimeSamples(
      { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothTime, 0 }, exchange, exchangeVoice, sampleRate);
  }
  else
  {
    for (int i = 0; i < FFEnvStageCount; i++)
      stageLengths.push_back(state->AudioParamBarsSamples(
        { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageBars, i }, exchange, exchangeVoice, sampleRate, bpm));
    smoothLength = state->AudioParamBarsSamples(
      { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothBars, 0 }, exchange, exchangeVoice, sampleRate, bpm);
  }
}

static FBModuleGraphPlotParams
PlotParams(FBGraphRenderState const* state)
{
  int smoothLength;
  std::vector<int> stageLengths;
  FBModuleGraphPlotParams result = {};
  StageLengthAudioSamples(state, false, -1, stageLengths, smoothLength);
  for (int i = 0; i < stageLengths.size(); i++)
    result.samples += stageLengths[i];
  result.samples += smoothLength;
  result.releaseAt = result.samples; // todo
  return result;
}

FFEnvProcessor& 
EnvGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.voice[state.voice->slot].env[state.moduleSlot].processor;
}

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  EnvGraphRenderData renderData = {};
  graphData->drawMarkers = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::Env;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].env[slot]; };
  renderData.voiceCVOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].env[slot].output; };
  
  FBRenderModuleGraph<false, false>(renderData, 0);
  FBTopoIndices indices = { (int)FFModuleType::Env, graphData->renderState->ModuleProcState()->moduleSlot };
  graphData->graphs[0].moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(indices)->name;  
}

void
EnvGraphRenderData::DoProcessIndicators(bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
{
  int smoothLengthAudio;
  int totalSamplesAudio = 0;
  std::vector<int> stageLengthsAudio;
  FBModuleGraphPlotParams result = {}; 
  StageLengthAudioSamples(graphData->renderState, exchange, exchangeVoice, stageLengthsAudio, smoothLengthAudio);
  for (int i = 0; i < stageLengthsAudio.size(); i++)
    totalSamplesAudio += stageLengthsAudio[i];
  totalSamplesAudio += smoothLengthAudio;
  float thisSamplesGUI = static_cast<float>(points.l.size());

  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  int releasePoint = graphData->renderState->AudioParamDiscrete(
    { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Release, 0 }, exchange, exchangeVoice);
  if (releasePoint != 0)
  {
    int releasePointSamples = 0;
    for (int i = 0; i < releasePoint - 1; i++)
      releasePointSamples += stageLengthsAudio[i];
    releasePointSamples = static_cast<int>(releasePointSamples * thisSamplesGUI / totalSamplesAudio);
    points.verticalIndicators.push_back(releasePointSamples);
  }

  int loopStart = graphData->renderState->AudioParamDiscrete(
    { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::LoopStart, 0 }, exchange, exchangeVoice);
  if (loopStart != 0)
  {
    int lp = 0;
    int loopStartSamples = 0;
    for (; lp < loopStart - 1; lp++)
      loopStartSamples += stageLengthsAudio[lp];
    loopStartSamples = static_cast<int>(loopStartSamples * thisSamplesGUI / totalSamplesAudio);
    points.verticalIndicators.push_back(loopStartSamples);

    int loopLength = graphData->renderState->AudioParamDiscrete(
      { (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::LoopLength, 0 }, exchange, exchangeVoice);
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