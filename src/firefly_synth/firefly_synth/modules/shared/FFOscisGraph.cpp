#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/shared/FFOscisGraph.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct OscisGraphRenderData final:
public FBModuleGraphRenderData<OscisGraphRenderData>
{
  FFVoiceDSPState& GetVoiceDSPState(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoPostProcess(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

FFVoiceDSPState&
OscisGraphRenderData::GetVoiceDSPState(FBModuleProcState& state)
{
  return state.ProcAs<FFProcState>()->dsp.voice[state.voice->slot];
}

void
OscisGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  if (graphIndex >= FFOsciCount)
  {
    // dealing with string osci
    int slot = graphIndex - FFOsciCount;
    auto& processor = GetVoiceDSPState(*moduleProcState).stringOsci[slot].processor;
    processor->InitializeBuffers(moduleProcState->input->sampleRate);
    processor->BeginVoice(true, *moduleProcState);
  } else
  {
    // dealing with osci + mod matrix
    // need to handle all oscis up to this one
    int slot = moduleProcState->moduleSlot;
    moduleProcState->moduleSlot = 0;
    GetVoiceDSPState(*moduleProcState).osciMod.processor->BeginVoice(true, *moduleProcState);
    for (int i = 0; i <= graphIndex; i++)
    {
      moduleProcState->moduleSlot = i;
      auto& processor = GetVoiceDSPState(*moduleProcState).osci[i].processor;
      processor->InitializeBuffers(true, moduleProcState->input->sampleRate);
      processor->BeginVoice(true, *moduleProcState);
    }
    moduleProcState->moduleSlot = slot;
  }
}

int 
OscisGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  if (graphIndex >= FFOsciCount)
  {
    // dealing with string osci
    int slot = graphIndex - FFOsciCount;
    return GetVoiceDSPState(*moduleProcState).stringOsci[slot].processor->Process(*moduleProcState);
  }
  else
  {
    // dealing with osci + mod matrix
    // need to handle all oscis up to this one
    int result = 0;
    int slot = moduleProcState->moduleSlot;
    moduleProcState->moduleSlot = 0;
    GetVoiceDSPState(*moduleProcState).osciMod.processor->Process(*moduleProcState);
    for (int i = 0; i <= graphIndex; i++)
    {
      moduleProcState->moduleSlot = i;
      int processed = GetVoiceDSPState(*moduleProcState).osci[i].processor->Process(*moduleProcState);
      if (i == graphIndex) result = processed;
    }
    moduleProcState->moduleSlot = slot;
    return result;
  }
}

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, int graphIndex)
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.staticModuleIndex = static_cast<int>(graphIndex >= FFOsciCount ? FFModuleType::Osci : FFModuleType::Osci);

  int rounds = graphIndex >= FFOsciCount ? FFStringOsciGraphRounds : 1;
  auto moduleType = graphIndex >= FFOsciCount ? FFModuleType::StringOsci : FFModuleType::Osci;
  int fineParam = graphIndex >= FFOsciCount ? (int)FFStringOsciParam::Fine : (int)FFOsciParam::Fine;
  int coarseParam = graphIndex >= FFOsciCount ? (int)FFStringOsciParam::Coarse : (int)FFOsciParam::Coarse;

  auto const* state = data->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { coarseParam, 0 } };
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear(indices, false, -1));
  pitch += state->AudioParamLinear({ { (int)moduleType, moduleSlot }, { fineParam, 0 } }, false, -1);
  result.sampleCount = FBFreqToSamples(FBPitchToFreq(pitch), sampleRate) * rounds;
  return result;
}

void
FFOscisRenderGraph(FBModuleGraphComponentData* graphData)
{
  OscisGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;

  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, int graphIndex) {
    auto& voiceExchange = static_cast<FFExchangeState const*>(exchangeState)->voice[voice];
    return graphIndex >= FFOsciCount ? &voiceExchange.stringOsci[slot] : &voiceExchange.osci[slot]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int slot, int graphIndex) {
    auto& voiceDSP = static_cast<FFProcState const*>(procState)->dsp.voice[voice];
    return graphIndex >= FFOsciCount ? &voiceDSP.stringOsci[slot].output : &voiceDSP.osci[slot].output; };

  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  for (int o = 0; o < FFOsciCount; o++)
  {
    graphData->renderState->ModuleProcState()->moduleSlot = o;
    FBRenderModuleGraph<false, true>(renderData, o);
    FBTopoIndices modIndices = { (int)FFModuleType::Osci, o };
    FBParamTopoIndices paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFOsciParam::Type, 0 } };
    graphData->graphs[o].text = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->graphName;
    if (graphData->renderState->AudioParamList<FFOsciType>(paramIndices, false, -1) == FFOsciType::Off)
      graphData->graphs[o].text += " OFF";
  }
  for (int o = 0; o < FFStringOsciCount; o++)
  {
    graphData->renderState->ModuleProcState()->moduleSlot = o;
    FBRenderModuleGraph<false, true>(renderData, FFOsciCount + o);
    FBTopoIndices modIndices = { (int)FFModuleType::StringOsci, o };
    FBParamTopoIndices paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFStringOsciParam::Type, 0 } };
    graphData->graphs[FFOsciCount + o].text = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->graphName;
    if (graphData->renderState->AudioParamList<FFStringOsciType>(paramIndices, false, -1) == FFStringOsciType::Off)
      graphData->graphs[FFOsciCount + o].text += " OFF";
  }
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}