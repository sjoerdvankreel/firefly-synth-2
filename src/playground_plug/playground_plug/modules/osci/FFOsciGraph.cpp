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
  result.samples = FBFreqToSamples(FBPitchToFreq(60.0f, sampleRate), sampleRate); // TODO actual pitch
  return result;
}

void
FFOsciRenderGraph(FBModuleGraphComponentData* graphData)
{
  FBModuleGraphRenderData<FFOsciProcessor> renderData = {};
  graphData->bipolar = true;
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::Osci;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].osci[slot]; };
  renderData.voiceOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].osci[slot].output[0]; }; // TODO L/R?
  FBRenderModuleGraph<false>(renderData);
}