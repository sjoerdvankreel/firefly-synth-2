#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/osci/FFOsciGraph.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>

struct OsciGraphRenderData final:
public FBModuleGraphRenderData<OsciGraphRenderData>
{
  FFVoiceDSPState& GetVoiceDSPState(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoPostProcess(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoProcessIndicators(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

FFVoiceDSPState&
OsciGraphRenderData::GetVoiceDSPState(FBModuleProcState& state)
{
  return state.ProcAs<FFProcState>()->dsp.voice[state.voice->slot];
}

void 
OsciGraphRenderData::DoReleaseOnDemandBuffers(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{
  // need to handle all oscis up to this one
  auto* moduleProcState = state->ModuleProcState();
  int slot = moduleProcState->moduleSlot;
  for (int i = 0; i <= graphIndex; i++)
  {
    auto& processor = GetVoiceDSPState(*moduleProcState).osci[i].processor;
    processor->ReleaseOnDemandBuffers(
      state->PlugGUI()->HostContext()->Topo(),
      state->ProcContainer());
  }
  moduleProcState->moduleSlot = slot;
}

void
OsciGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{
  // need to handle all oscis up to this one + mod matrix
  auto* moduleProcState = state->ModuleProcState();
  int slot = moduleProcState->moduleSlot;
  moduleProcState->moduleSlot = 0;
  GetVoiceDSPState(*moduleProcState).osciMod.processor->BeginVoice(*moduleProcState, true);
  for (int i = 0; i <= graphIndex; i++)
  {
    moduleProcState->moduleSlot = i;
    auto& processor = GetVoiceDSPState(*moduleProcState).osci[i].processor;
    processor->AllocOnDemandBuffers(
      state->PlugGUI()->HostContext()->Topo(), 
      state->ProcContainer(), 
      i, true, moduleProcState->input->sampleRate);
    processor->BeginVoice(*moduleProcState, true);
  }
  moduleProcState->moduleSlot = slot;
}

int 
OsciGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{
  // need to handle all oscis up to this one + mod matrix
  auto* moduleProcState = state->ModuleProcState();
  int result = 0;
  int slot = moduleProcState->moduleSlot;
  moduleProcState->moduleSlot = 0;
  GetVoiceDSPState(*moduleProcState).osciMod.processor->Process(*moduleProcState);
  for (int i = 0; i <= graphIndex; i++)
  {
    moduleProcState->moduleSlot = i;
    int processed = GetVoiceDSPState(*moduleProcState).osci[i].processor->Process(*moduleProcState, true);
    if (i == graphIndex) result = processed;
  }
  moduleProcState->moduleSlot = slot;
  return result;
}

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, int /*graphIndex*/)
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.staticModuleIndex = static_cast<int>(FFModuleType::Osci);

  auto const* state = data->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float sampleRate = state->ExchangeContainer()->Host()->sampleRate;
  FBParamTopoIndices indices = { { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Coarse, 0 } };
  float pitch = 60.0f + static_cast<float>(state->AudioParamLinear(indices, false, -1));
  pitch += state->AudioParamLinear({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Fine, 0 } }, false, -1);
  auto type = state->AudioParamList<FFOsciType>({ { (int)FFModuleType::Osci, moduleSlot }, { (int)FFOsciParam::Type, 0 } }, false, -1);
  int rounds = type == FFOsciType::String? FFOsciStringGraphRounds : 1;
  result.sampleCount = FBFreqToSamples(FBPitchToFreq(pitch), sampleRate) * rounds;
  return result;
}

void
FFOsciRenderGraph(FBModuleGraphComponentData* graphData)
{
  OsciGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;

  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].osci[slot]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].osci[slot].output; };

  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  for (int o = 0; o < FFOsciCount; o++)
  {
    graphData->renderState->ModuleProcState()->moduleSlot = o;
    FBRenderModuleGraph<false, true>(renderData, o);
    FBTopoIndices modIndices = { (int)FFModuleType::Osci, o };
    FBParamTopoIndices paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFOsciParam::Type, 0 } };
    graphData->graphs[o].text = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
    auto osciType = graphData->renderState->AudioParamList<FFOsciType>(paramIndices, false, -1);
    graphData->graphs[o].text += " " + FFOsciTypeToString(osciType);
  }
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}