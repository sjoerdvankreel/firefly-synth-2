#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciGraph.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

static FBModuleGraphPlotParams
PlotParams(FBGraphRenderState const* state)
{
  FBModuleGraphPlotParams result = {};
  result.releaseAt = -1;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Proc()->sampleRate;
  int unison = state->AudioParamDiscrete({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::UnisonCount, 0 });
  float pitch = static_cast<float>(state->AudioParamNote({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 }));
  pitch += state->AudioParamLinear({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  result.samples = FBFreqToSamples(FBPitchToFreq(pitch, sampleRate), sampleRate);
  result.seriesMultiplier = 1.0f / (unison * (1.0f / std::sqrt(static_cast<float>(unison))));
  return result;
}

void
FFOsciRenderGraph(FBModuleGraphComponentData* graphData)
{
  FBModuleGraphRenderData<FFOsciProcessor> renderData = {};
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