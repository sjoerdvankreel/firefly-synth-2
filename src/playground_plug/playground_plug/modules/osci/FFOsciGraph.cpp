#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciGraph.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct OsciGraphRenderData final:
public FBModuleGraphRenderData<OsciGraphRenderData>
{
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
  FFOsciProcessor& GetProcessor(FBModuleProcState& state);
};

void
OsciGraphRenderData::BeginVoice(FBModuleProcState& state)
{
  int highestSlot = state.moduleSlot;
  for (int i = 0; i <= highestSlot; i++)
  {
    state.moduleSlot = i;
    GetProcessor(state).BeginVoice(state);
  }
}

int 
OsciGraphRenderData::Process(FBModuleProcState& state)
{
  int result = 0;
  int highestSlot = state.moduleSlot;
  for (int i = 0; i <= highestSlot; i++)
  {
    state.moduleSlot = i;
    result = GetProcessor(state).Process(state);
  }
  return result;
}

FFOsciProcessor&
OsciGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return procState->dsp.voice[state.voice->slot].osci[state.moduleSlot].processor;
}

static FBModuleGraphPlotParams
PlotParams(FBGraphRenderState const* state)
{
  FBModuleGraphPlotParams result = {};
  result.releaseAt = -1;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  int unison = state->AudioParamDiscrete({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UnisonCount, 0 });
  float pitch = static_cast<float>(state->AudioParamNote({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 }));
  pitch += state->AudioParamLinear({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  result.samples = FBFreqToSamples(FBPitchToFreqAccurate(pitch, sampleRate), sampleRate);
  result.seriesMultiplier = 1.0f / (unison * (1.0f / std::sqrt(static_cast<float>(unison))));
  return result;
}

void
FFOsciRenderGraph(FBModuleGraphComponentData* graphData)
{
  OsciGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->skipDrawOnEqualsPrimary = false;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::Osci;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].osci[slot]; };
  renderData.voiceAudioOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].osci[slot].output; };
  FBRenderModuleGraph<false, true>(renderData);
}