#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/osci/FFOsciGraph.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/gui/graph/FBGraphing.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <algorithm>
   
class OsciGraphProcessor final:
public FBModuleGraphProcessor
{
public:
  OsciGraphProcessor(FBModuleGraphComponentData* componentData) :
  FBModuleGraphProcessor(componentData) {}

  FFOsciProcessor& GetProcessor(FBModuleProcState& state);
  FFVoiceDSPState& GetVoiceDSPState(FBModuleProcState& state);

  FBModuleGraphPlotParams PlotParams(
    bool detailGraphs, int graphIndex) const override;
  FBSArray2<float, FBFixedBlockSamples, 2> const* StereoOutput(
    void const* procState, FBModuleGraphStateParams const& params);
  FBModuleProcExchangeStateBase const* ExchangeState(
    void const* exchangeState, FBModuleGraphStateParams const& params);

  int Process(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void BeginVoiceOrBlock(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void ReleaseOnDemandBuffers(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void ProcessIndicators(FBGraphRenderState* /*state*/,
    FBModuleGraphProcessParams const& /*params*/, FBModuleGraphPoints& /*points*/) override { }
  void PostProcessMarker(FBGraphRenderState* /*state*/,
    FBModuleGraphData& /*data*/, FBModuleGraphProcessParams const& /*params*/, float& /*positionNormalized*/, bool& /*displayMarker*/) override { }
  void PostProcess(FBGraphRenderState* state,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) override;
  void ProcessExchangeState(FBGraphRenderState* graphState,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState) override;
};

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

FFVoiceDSPState&
OsciGraphProcessor::GetVoiceDSPState(FBModuleProcState& state)
{
  return state.ProcAs<FFProcState>()->dsp.voice[state.voice->slot];
}

FBSArray2<float, FBFixedBlockSamples, 2> const*
OsciGraphProcessor::StereoOutput(
  void const* procState, FBModuleGraphStateParams const& params)
{
  return &static_cast<FFProcState const*>(procState)->dsp.voice[params.voice].osci[params.moduleSlot].output;
}

FBModuleProcExchangeStateBase const*
OsciGraphProcessor::ExchangeState(
  void const* exchangeState, FBModuleGraphStateParams const& params)
{
  return &static_cast<FFExchangeState const*>(exchangeState)->voice[params.voice].osci[params.moduleSlot];
}

void
OsciGraphProcessor::PostProcess(
  FBGraphRenderState* /*state*/, FBModuleGraphData& /*data*/,
  FBModuleGraphProcessParams const& /*params*/, FBModuleGraphPoints& points)
{
  points.bipolar = true;
}

void 
OsciGraphProcessor::ProcessExchangeState(
  FBGraphRenderState* /*graphState*/, FBModuleGraphData& data,
  FBModuleGraphProcessParams const& /*params*/, FBModuleProcExchangeStateBase const* exchangeState)
{
  auto osciExchange = dynamic_cast<FFOsciExchangeState const*>(exchangeState);
  data.exchangeMainText = FBPitchToStringNotes(osciExchange->osciEffectivePitch);
  data.exchangeGainValue = std::max(data.exchangeGainValue, osciExchange->output);
}

void 
OsciGraphProcessor::ReleaseOnDemandBuffers(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& params)
{
  // need to handle all oscis up to this one
  auto* moduleProcState = state->ModuleProcState();
  int slot = moduleProcState->moduleSlot;
  for (int i = 0; i <= params.graphIndex; i++)
  {
    auto& processor = GetVoiceDSPState(*moduleProcState).osci[i].processor;
    processor->ReleaseOnDemandBuffers(
      state->PlugGUI()->HostContext()->Topo(),
      state->ProcContainer());
  }
  moduleProcState->moduleSlot = slot;
}

void
OsciGraphProcessor::BeginVoiceOrBlock(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& params)
{
  // need to handle all oscis up to this one + mod matrix
  auto* moduleProcState = state->ModuleProcState();
  int slot = moduleProcState->moduleSlot;
  moduleProcState->moduleSlot = 0;
  GetVoiceDSPState(*moduleProcState).osciMod.processor->BeginVoice(*moduleProcState, true);
  for (int i = 0; i <= slot; i++)
  {
    moduleProcState->moduleSlot = i;
    FFOsciExchangeState const* exchangeFromDSP = GetOsciExchangeStateFromDSP(state, i, params.exchange, params.exchangeVoice);
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
OsciGraphProcessor::Process(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& params)
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
    FFOsciExchangeState const* exchangeFromDSP = GetOsciExchangeStateFromDSP(state, i, params.exchange, params.exchangeVoice);
    int processed = GetVoiceDSPState(*moduleProcState).osci[i].processor->Process(*moduleProcState, exchangeFromDSP, true);
    if (i == slot) result = processed;
  }
  moduleProcState->moduleSlot = slot;
  return result;
}

FBModuleGraphPlotParams
OsciGraphProcessor::PlotParams(bool /*detailGraphs*/, int /*graphIndex*/) const
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.staticModuleIndex = static_cast<int>(FFModuleType::Osci);

  auto const* state = ComponentData()->renderState;
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
  OsciGraphProcessor processor(graphData);
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent

  int moduleSlot = graphData->renderState->ModuleProcState()->moduleSlot;
  int graphCount = detailGraphs ? FFOsciCount : 1;
  for (int i = 0; i < graphCount; i++)
  {
    int graphModuleSlot = detailGraphs ? i : moduleSlot;
    graphData->renderState->ModuleProcState()->moduleSlot = graphModuleSlot;
    FBRenderModuleGraph(&processor, false, true, detailGraphs, i);
    FBTopoIndices modIndices = { (int)FFModuleType::Osci, graphModuleSlot };
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
    graphData->graphs[i].displayGainAsDb = true;
    graphData->graphs[i].defaultGainValue = gain;
    graphData->graphs[i].hasDefaultGainValue = true;
    graphData->graphs[i].defaultMainText = FBPitchToStringSemis(coarse, fine, 2, true);

    if (!detailGraphs)
      graphData->graphs[i].MergeStereo();
    graphData->graphs[i].ScaleToSelfNormalized();
  }
  graphData->renderState->ModuleProcState()->moduleSlot = moduleSlot;
}