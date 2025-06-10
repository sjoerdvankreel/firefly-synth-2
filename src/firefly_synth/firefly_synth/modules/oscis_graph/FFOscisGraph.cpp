#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>
#include <firefly_synth/modules/oscis_graph/FFOscisGraph.hpp>

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
  void DoBeginVoiceOrReset(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
  void DoPostProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

FFVoiceDSPState&
OscisGraphRenderData::GetVoiceDSPState(FBModuleProcState& state)
{
  return state.ProcAs<FFProcState>()->dsp.voice[state.voice->slot];
}

void
OscisGraphRenderData::DoBeginVoiceOrReset(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  int osciSlot = moduleProcState->moduleSlot;
  assert(osciSlot == graphIndex);
  moduleProcState->moduleSlot = 0;
  GetVoiceDSPState(*moduleProcState).osciMod.processor->BeginVoice(true, *moduleProcState);
  for (int i = 0; i <= graphIndex; i++)
  {
    moduleProcState->moduleSlot = i;
    GetVoiceDSPState(*moduleProcState).osci[i].processor->BeginVoice(true, *moduleProcState);
  }
  moduleProcState->moduleSlot = osciSlot;
}

int 
OscisGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  int result = 0;
  auto* moduleProcState = state->ModuleProcState();
  int osciSlot = moduleProcState->moduleSlot;
  assert(osciSlot == graphIndex);
  moduleProcState->moduleSlot = 0;
  GetVoiceDSPState(*moduleProcState).osciMod.processor->Process(*moduleProcState);
  for (int i = 0; i <= graphIndex; i++)
  {
    moduleProcState->moduleSlot = i;
    int processed = GetVoiceDSPState(*moduleProcState).osci[i].processor->Process(*moduleProcState);
    if (i == osciSlot)
      result = processed;
  }
  moduleProcState->moduleSlot = osciSlot;
  return result;
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
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Coarse, 0 }, false, -1));
  pitch += state->AudioParamLinear({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Fine, 0 }, false, -1);
  result.sampleCount = FBFreqToSamples(FBPitchToFreq(pitch), sampleRate);
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
  renderData.staticModuleIndex = (int)FFModuleType::Osci;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].osci[slot]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].osci[slot].output; };
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  for (int o = 0; o < FFOsciCount; o++)
  {
    graphData->renderState->ModuleProcState()->moduleSlot = o;
    FBRenderModuleGraph<false, true>(renderData, o);
    FBTopoIndices modIndices = { (int)FFModuleType::Osci, o };
    FBParamTopoIndices paramIndices = { modIndices.index, modIndices.slot, (int)FFOsciParam::Type, 0 };
    graphData->graphs[o].text = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
    if (graphData->renderState->AudioParamList<FFOsciType>(paramIndices, false, -1) == FFOsciType::Off)
      graphData->graphs[o].text += " OFF";
  }
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}