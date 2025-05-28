#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModProcessor.hpp>
#include <playground_plug/modules/oscis_graph/FFOscisGraph.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGraphing.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct OscisGraphRenderData final:
public FBModuleGraphRenderData<OscisGraphRenderData>
{
  int DoProcess(FBModuleProcState& state);
  void DoBeginVoice(FBModuleProcState& state);
  FFVoiceDSPState& GetVoiceDSPState(FBModuleProcState& state);
  void DoProcessIndicators(bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
};

void
OscisGraphRenderData::DoBeginVoice(FBModuleProcState& state)
{
  int osciSlot = state.moduleSlot;
  state.moduleSlot = 0;
  GetVoiceDSPState(state).osciMod.processor->BeginVoice(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    GetVoiceDSPState(state).osci[i].processor->BeginVoice(state);
  }
  state.moduleSlot = osciSlot;
}

int 
OscisGraphRenderData::DoProcess(FBModuleProcState& state)
{
  int result = 0;
  int osciSlot = state.moduleSlot;
  state.moduleSlot = 0;
  GetVoiceDSPState(state).osciMod.processor->Process(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    int processed = GetVoiceDSPState(state).osci[i].processor->Process(state);
    if (i == osciSlot)
      result = processed;
  }
  state.moduleSlot = osciSlot;
  return result;
}

FFVoiceDSPState&
OscisGraphRenderData::GetVoiceDSPState(FBModuleProcState& state)
{
  return state.ProcAs<FFProcState>()->dsp.voice[state.voice->slot];
}

static int
PlotSamples(FBGraphRenderState const* state)
{
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Coarse, 0 }, false, -1));
  pitch += state->AudioParamLinear({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Fine, 0 }, false, -1);
  return FBFreqToSamples(FBPitchToFreq(pitch), sampleRate);
}

void
FFOscisRenderGraph(FBModuleGraphComponentData* graphData)
{
  OscisGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false;
  renderData.graphData = graphData;
  renderData.plotSamplesSelector = PlotSamples;
  renderData.staticModuleIndex = (int)FFModuleType::Osci;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].osci[slot]; };
  renderData.voiceAudioOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].osci[slot].output; };
  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  for (int o = 0; o < FFOsciCount; o++)
  {
    graphData->renderState->ModuleProcState()->moduleSlot = o;
    FBTopoIndices indices = { (int)FFModuleType::Osci, o };
    graphData->graphs[o].moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(indices)->name;
    FBRenderModuleGraph<false, true>(renderData, o);
  }
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}