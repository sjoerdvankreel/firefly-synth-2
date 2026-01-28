#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/osci/FFOsciGraph.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>
   
struct OsciGraphRenderData final:
public FBModuleGraphRenderData<OsciGraphRenderData>
{
  FFVoiceDSPState& GetVoiceDSPState(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoPostProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoProcessExchangeState(FBGraphRenderState* graphState, FBModuleGraphData& data, bool detailGraphs, int graphIndex, int exchangeVoice, FBModuleProcExchangeStateBase const* exchangeState);
  void DoProcessIndicators(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

FFVoiceDSPState&
OsciGraphRenderData::GetVoiceDSPState(FBModuleProcState& state)
{
  return state.ProcAs<FFProcState>()->dsp.voice[state.voice->slot];
}

static FFOsciExchangeState const*
GetOsciExchangeStateFromDSP(FBGraphRenderState* state, int slot, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  FFOsciExchangeState const* exchangeFromDSP = nullptr;
  int runtimeModuleIndex = moduleProcState->topo->moduleTopoToRuntime.at({ (int)FFModuleType::Osci, slot });
  auto const* moduleExchangeState = state->ExchangeContainer()->Modules()[runtimeModuleIndex].get();
  if (exchange)
    exchangeFromDSP = &dynamic_cast<FFOsciExchangeState const&>(*moduleExchangeState->Voice()[exchangeVoice]);
  return exchangeFromDSP;
}

void
OsciGraphRenderData::DoPostProcess(
  FBGraphRenderState* /*state*/,
  bool /*detailGraphs*/, int /*graphIndex*/,
  bool /*exchange*/, int /*exchangeVoice*/,
  FBModuleGraphPoints& points)
{
  points.bipolar = true;
}

void 
OsciGraphRenderData::DoProcessExchangeState(
  FBGraphRenderState* /*graphState*/, FBModuleGraphData& data,
  bool /*detailGraphs*/, int /*graphIndex*/, int /*exchangeVoice*/,
  FBModuleProcExchangeStateBase const* exchangeState)
{
  auto osciExchange = dynamic_cast<FFOsciExchangeState const*>(exchangeState);
  data.exchangeMainText = FBPitchToStringNotes(osciExchange->osciEffectivePitch);
}

void 
OsciGraphRenderData::DoReleaseOnDemandBuffers(
  FBGraphRenderState* state, bool /*detailGraphs*/, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
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
  FBGraphRenderState* state, bool /*detailGraphs*/, int /*graphIndex*/, bool exchange, int exchangeVoice)
{
  // need to handle all oscis up to this one + mod matrix
  auto* moduleProcState = state->ModuleProcState();
  int slot = moduleProcState->moduleSlot;
  moduleProcState->moduleSlot = 0;
  GetVoiceDSPState(*moduleProcState).osciMod.processor->BeginVoice(*moduleProcState, true);
  for (int i = 0; i <= slot; i++)
  {
    moduleProcState->moduleSlot = i;
    FFOsciExchangeState const* exchangeFromDSP = GetOsciExchangeStateFromDSP(state, i, exchange, exchangeVoice);
    auto& processor = GetVoiceDSPState(*moduleProcState).osci[i].processor;
    processor->AllocOnDemandBuffers(
      state->PlugGUI()->HostContext()->Topo(), 
      state->ProcContainer(), 
      i, true, moduleProcState->input->sampleRate);
    processor->BeginVoice(*moduleProcState, exchangeFromDSP, true);
  }
  moduleProcState->moduleSlot = slot;
}

int 
OsciGraphRenderData::DoProcess(
  FBGraphRenderState* state, bool /*detailGraphs*/, int /*graphIndex*/, bool exchange, int exchangeVoice)
{
  // need to handle all oscis up to this one + mod matrix
  auto* moduleProcState = state->ModuleProcState();
  int result = 0;
  int slot = moduleProcState->moduleSlot;
  moduleProcState->moduleSlot = 0;
  GetVoiceDSPState(*moduleProcState).osciMod.processor->Process(*moduleProcState);
  for (int i = 0; i <= slot; i++)
  {
    moduleProcState->moduleSlot = i;
    FFOsciExchangeState const* exchangeFromDSP = GetOsciExchangeStateFromDSP(state, i, exchange, exchangeVoice);
    int processed = GetVoiceDSPState(*moduleProcState).osci[i].processor->Process(*moduleProcState, exchangeFromDSP, true);
    if (i == slot) result = processed;
  }
  moduleProcState->moduleSlot = slot;
  return result;
}

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, bool /*detailGraphs*/, int /*graphIndex*/)
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

  auto freq = FBPitchToFreq(pitch);
  auto samples = FBFreqToSamples(freq, sampleRate);
  result.sampleCount = samples * rounds;
  return result;
}

void
FFOsciRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  OsciGraphRenderData renderData = {};
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;

  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].osci[slot]; };
  renderData.voiceStereoOutputSelector = [](void const* procState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].osci[slot].output; };

  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  int graphCount = detailGraphs ? FFOsciCount : 1;
  for (int i = 0; i < graphCount; i++)
  {
    int graphModuleSlot = detailGraphs ? i : moduleSlot;
    graphData->renderState->ModuleProcState()->moduleSlot = graphModuleSlot;
    FBRenderModuleGraph<false, true>(renderData, detailGraphs, i);
    FBTopoIndices modIndices = { (int)FFModuleType::Osci, graphModuleSlot };
    
    int maxSizeAllSeries = 0;
    float absMaxValueAllSeries = 0.0f;
    graphData->graphs[i].GetLimits(false, maxSizeAllSeries, absMaxValueAllSeries);
    
    FBParamTopoIndices paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFOsciParam::Type, 0 } };
    auto osciType = graphData->renderState->AudioParamList<FFOsciType>(paramIndices, false, -1);
    paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFOsciParam::UniCount, 0 } };
    int uniCount = graphData->renderState->AudioParamDiscrete(paramIndices, false, -1);
    paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFOsciParam::Gain, 0 } };
    float gain = graphData->renderState->AudioParamLinear(paramIndices, false, -1);
    paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFOsciParam::Coarse, 0 } };
    float coarse = graphData->renderState->AudioParamLinear(paramIndices, false, -1);
    paramIndices = { { modIndices.index, modIndices.slot }, { (int)FFOsciParam::Fine, 0 } };
    float fine = graphData->renderState->AudioParamLinear(paramIndices, false, -1);

    std::string title = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
    title += ": " + FFOsciTypeToString(osciType);
    if (osciType != FFOsciType::Off)
    {
      title += ", " + std::to_string(uniCount) + " Voice";
      if (uniCount > 1)
        title += "s";
    }
    
    graphData->graphs[i].title = title;
    graphData->graphs[i].moduleSlot = graphModuleSlot;
    graphData->graphs[i].moduleIndex = (int)FFModuleType::Osci;
    graphData->graphs[i].defaultSubText = FBGainToStringDb(gain, 2);
    graphData->graphs[i].defaultMainText = FBPitchToStringSemis(coarse, fine, 2);
    graphData->graphs[i].exchangeSubText = FBGainToStringDb(absMaxValueAllSeries, 2);

    if (!detailGraphs)
      graphData->graphs[i].MergeStereo();
    graphData->graphs[i].ScaleToSelfNormalized();
  }
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}