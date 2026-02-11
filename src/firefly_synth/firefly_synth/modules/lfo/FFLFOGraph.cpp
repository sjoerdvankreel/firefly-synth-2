#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/lfo/FFLFOGraph.hpp>
#include <firefly_synth/modules/lfo/FFLFOProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

template <bool Global>
class LFOGraphProcessor final:
public FBModuleGraphProcessor
{
public:
  int totalSamples = {};
  std::array<int, FFLFOBlockCount> samplesProcessed = {};

  FFLFOProcessor& GetProcessor(FBModuleProcState& state);
  LFOGraphProcessor(FBModuleGraphComponentData* componentData) :
  FBModuleGraphProcessor(componentData) {}

  FBModuleGraphPlotParams PlotParams(
    bool detailGraphs, int graphIndex) const override;
  FBSArray<float, FBFixedBlockSamples> const* MonoOutput(
    void const* procState, FBModuleGraphStateParams const& params);
  FBModuleProcExchangeStateBase const* ExchangeState(
    void const* exchangeState, FBModuleGraphStateParams const& params);

  int Process(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void BeginVoiceOrBlock(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) override;
  void ReleaseOnDemandBuffers(FBGraphRenderState* /*state*/,
    FBModuleGraphProcessParams const& /*params*/) override { }
  void ProcessIndicators(FBGraphRenderState* /*state*/,
    FBModuleGraphProcessParams const& /*params*/, FBModuleGraphPoints& /*points*/) override { }
  void PostProcess(FBGraphRenderState* /*state*/,
    FBModuleGraphData& /*data*/, FBModuleGraphProcessParams const& /*params*/, FBModuleGraphPoints& /*points*/) override { }
  void ProcessExchangeState(FBGraphRenderState* graphState,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState) override;
};

template <bool Global>
FFLFOProcessor&
LFOGraphProcessor<Global>::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *FFSelectDualState<Global>(
    [procState, &state] { return procState->dsp.global.gLFO[state.moduleSlot].processor.get(); },
    [procState, &state] { return procState->dsp.voice[state.voice->slot].vLFO[state.moduleSlot].processor.get(); });
}

template <bool Global>
FBSArray<float, FBFixedBlockSamples> const* 
LFOGraphProcessor<Global>::MonoOutput(
  void const* procState, FBModuleGraphStateParams const& params)
{
  if constexpr(Global)
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[params.moduleSlot].outputAll;
  else
    return &static_cast<FFProcState const*>(procState)->dsp.voice[params.voice].vLFO[params.moduleSlot].outputAll;
}

template <bool Global>
FBModuleProcExchangeStateBase const* 
LFOGraphProcessor<Global>::ExchangeState(
  void const* exchangeState, FBModuleGraphStateParams const& params)
{
  if constexpr(Global)
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gLFO[params.moduleSlot];
  else
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[params.voice].vLFO[params.moduleSlot];
}

template <bool Global>
FBModuleGraphPlotParams
LFOGraphProcessor<Global>::PlotParams(
  bool detailGraphs, int graphIndex) const
{
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = ComponentData()->pixelWidth;
  result.staticModuleIndex = static_cast<int>(Global ? FFModuleType::GLFO : FFModuleType::VLFO);
  
  int hostSampleCount;
  auto const* state = ComponentData()->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float bpm = state->ExchangeContainer()->Host()->bpm;
  float hostSampleRate = state->ExchangeContainer()->Host()->sampleRate;
  int moduleType = (int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO);
  FBParamTopoIndices indices = { { moduleType, moduleSlot }, { (int)FFLFOParam::Sync, 0 } };
  bool sync = state->AudioParamBool(indices, false, -1);
  if (sync)
  {
    indices = { { moduleType, moduleSlot }, { (int)FFLFOParam::RateBars, graphIndex } };
    hostSampleCount = state->AudioParamBarsSamples(indices, false, -1, hostSampleRate, bpm);
  }
  else
  {
    indices = { { moduleType, moduleSlot }, { (int)FFLFOParam::RateHz, graphIndex } };
    hostSampleCount = state->AudioParamLinearFreqSamples(indices, false, -1, hostSampleRate);
  }
  result.sampleRate = result.sampleCount * hostSampleRate / hostSampleCount;
  return result;
}

template <bool Global>
void 
LFOGraphProcessor<Global>::BeginVoiceOrBlock(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& params)
{ 
  samplesProcessed[params.graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  FFLFOExchangeState const* exchangeFromDSP = nullptr;
  int moduleSlot = moduleProcState->moduleSlot;
  int staticModuleIndex = (int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO);
  int runtimeModuleIndex = moduleProcState->topo->moduleTopoToRuntime.at({ staticModuleIndex, moduleSlot });
  auto const* moduleExchangeState = state->ExchangeContainer()->Modules()[runtimeModuleIndex].get();
  if (params.exchange)
    if constexpr (Global)
      exchangeFromDSP = &dynamic_cast<FFLFOExchangeState const&>(*moduleExchangeState->Global());
    else
      exchangeFromDSP = &dynamic_cast<FFLFOExchangeState const&>(*moduleExchangeState->Voice()[params.exchangeVoice]);
  GetProcessor(*moduleProcState).template BeginVoiceOrBlock<Global>(*moduleProcState, exchangeFromDSP, true, params.detailGraphs, params.graphIndex, totalSamples);
}

template <bool Global>
int 
LFOGraphProcessor<Global>::Process(
  FBGraphRenderState* state, 
  FBModuleGraphProcessParams const& params)
{
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GLFO : FFModuleType::VLFO;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } };
  auto type = state->AudioParamList<FFLFOType>(indices, false, -1);
  if (type == FFLFOType::Off)
    return 0;

  if (params.detailGraphs)
  {
    indices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, params.graphIndex } };
    auto opType = state->AudioParamList<FFModulationOpType>(indices, params.exchange, params.exchangeVoice);
    if (opType == FFModulationOpType::Off)
      return 0;
  }

  samplesProcessed[params.graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).template Process<Global>(*moduleProcState, true);
}

template <bool Global>
void
LFOGraphProcessor<Global>::ProcessExchangeState(
  FBGraphRenderState* graphState, FBModuleGraphData& data,
  FBModuleGraphProcessParams const& params, 
  FBModuleProcExchangeStateBase const* exchangeState)
{
  auto lfoExchange = dynamic_cast<FFLFOExchangeState const*>(exchangeState);
  if (!params.detailGraphs)
  {
    data.exchangeMainText = FBToStringHz(lfoExchange->rates[0], 2);
    data.exchangeGainValue = std::max(data.exchangeGainValue, lfoExchange->output);
    return;
  }

  data.exchangeMainText = FBToStringHz(lfoExchange->rates[params.graphIndex], 2);
  data.exchangeGainValue = std::max(data.exchangeGainValue, lfoExchange->outputs[params.graphIndex]);
}

template <bool Global>
void
FFLFORenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  LFOGraphProcessor<Global> processor(graphData);
  auto moduleType = Global ? FFModuleType::GLFO : FFModuleType::VLFO;
  graphData->skipDrawOnEqualsPrimary = false; // need exchange state for all sub-lfos
  graphData->drawMarkersSelector = [](int) { return true; };
  processor.totalSamples = processor.PlotParams(detailGraphs, 0).sampleCount; // just pick one

  auto* renderState = graphData->renderState;
  auto* moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBTopoIndices modIndices = { (int)moduleType, moduleSlot };
  FBParamTopoIndices paramIndices = { modIndices, { (int)FFLFOParam::Sync, 0 } };
  bool sync = renderState->AudioParamBool(paramIndices, false, -1);
  auto const* topo = graphData->renderState->PlugGUI()->HostContext()->Topo();
  auto const& moduleTopo = topo->static_->modules[(int)moduleType];
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
  int graphCount = detailGraphs ? FFLFOBlockCount : 1;

  for (int i = 0; i < graphCount; i++)
  {
    FBRenderModuleGraph(&processor, Global, false, detailGraphs, i);
    graphData->graphs[i].moduleSlot = moduleSlot;
    graphData->graphs[i].moduleIndex = (int)moduleType;
    if (detailGraphs)
    {
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, i } };
      auto opType = renderState->AudioParamList<FFModulationOpType>(paramIndices, false, -1);
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::WaveMode, i } };
      auto waveMode = renderState->AudioParamList<FFLFOWaveMode>(paramIndices, false, -1);
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateHz, i } };
      float rateHz = renderState->AudioParamLinear(paramIndices, false, -1);
      paramIndices = { modIndices, { (int)FFLFOParam::RateBars, i } };
      double rateBarsNorm = renderState->AudioParamNormalized(paramIndices, false, -1);
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Min, i } };
      float minVal = renderState->AudioParamIdentity(paramIndices, false, -1);
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Max, i } };
      float maxVal = renderState->AudioParamIdentity(paramIndices, false, -1);
      graphData->graphs[i].title = std::string(1, static_cast<char>('A' + i));
      graphData->graphs[i].title += ": " + FFModulationOpTypeToString(opType);
      if(opType != FFModulationOpType::Off)
        graphData->graphs[i].title += ", " + FFLFOWaveModeToString(waveMode);
      graphData->graphs[i].hasDefaultGainValue = true;
      graphData->graphs[i].defaultGainValue = std::max(minVal, maxVal);
      graphData->graphs[i].defaultMainText = !sync ?
        FBToStringHz(rateHz, 2):
        (moduleTopo.params[(int)FFLFOParam::RateBars].BarsNonRealTime().NormalizedToText(false, 0, rateBarsNorm) + " Bars");
    }
    else
    {
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } };
      auto type = renderState->AudioParamList<FFLFOType>(paramIndices, false, -1);
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateHz, 0 } };
      float rateHz = renderState->AudioParamLinear(paramIndices, false, -1);
      paramIndices = { modIndices, { (int)FFLFOParam::RateBars, 0 } };
      double rateBarsNorm = renderState->AudioParamNormalized(paramIndices, false, -1);
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Min, 0 } };
      float minVal = renderState->AudioParamIdentity(paramIndices, false, -1);
      paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Max, 0 } };
      float maxVal = renderState->AudioParamIdentity(paramIndices, false, -1);
      graphData->graphs[0].title = moduleName;
      graphData->graphs[0].title += ": " + FFLFOTypeToString(type, Global);
      if (type != FFLFOType::Off)
        graphData->graphs[0].title += std::string(", ") + (sync ? "BPM" : "Time");
      graphData->graphs[0].hasDefaultGainValue = true;
      graphData->graphs[0].defaultGainValue = std::max(minVal, maxVal);
      graphData->graphs[0].defaultMainText = !sync ?
        FBToStringHz(rateHz, 2) :
        (moduleTopo.params[(int)FFLFOParam::RateBars].BarsNonRealTime().NormalizedToText(false, 0, rateBarsNorm) + " Bars");
    }
    graphData->graphs[i].ScaleToSelfNormalized();
  }
}

template void FFLFORenderGraph<true>(FBModuleGraphComponentData*, bool);
template void FFLFORenderGraph<false>(FBModuleGraphComponentData*, bool);