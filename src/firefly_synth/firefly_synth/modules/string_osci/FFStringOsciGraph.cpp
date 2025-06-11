#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciGraph.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct StringOsciGraphRenderData final :
public FBModuleGraphRenderData<StringOsciGraphRenderData>
{
  FFStringOsciProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
  void DoPostProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

int 
StringOsciGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  return GetProcessor(*moduleProcState).Process(*moduleProcState);
}

void 
StringOsciGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).BeginVoice(true, *moduleProcState);
}

FFStringOsciProcessor&
StringOsciGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& processor = *procState->dsp.voice[state.voice->slot].stringOsci[state.moduleSlot].processor;
  processor.InitializeBuffers(true, state.input->sampleRate);
  return processor;
}

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data)
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;

  auto const* state = data->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  FBParamTopoIndices indices = { (int)FFModuleType::StringOsci, moduleSlot, (int)FFStringOsciParam::Coarse, 0 };
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear(indices, false, -1));
  pitch += state->AudioParamLinear({ (int)FFModuleType::StringOsci, moduleSlot, (int)FFStringOsciParam::Fine, 0 }, false, -1);
  result.sampleCount = FBFreqToSamples(FBPitchToFreq(pitch), sampleRate) * FFStringOsciGraphRounds;
  return result;
}

void
FFStringOsciRenderGraph(FBModuleGraphComponentData* graphData)
{
  StringOsciGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.staticModuleIndex = (int)FFModuleType::StringOsci;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].stringOsci[slot]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].stringOsci[slot].output; };

  FBRenderModuleGraph<false, true>(renderData, 0);
  FBTopoIndices modIndices = { (int)FFModuleType::StringOsci, graphData->renderState->ModuleProcState()->moduleSlot };
  FBParamTopoIndices paramIndices = { modIndices.index, modIndices.slot, (int)FFStringOsciParam::Type, 0 };
  graphData->graphs[0].text = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
  if (graphData->renderState->AudioParamList<FFStringOsciType>(paramIndices, false, -1) == FFStringOsciType::Off)
    graphData->graphs[0].text += " OFF";
}