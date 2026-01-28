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
struct LFOGraphRenderData final:
public FBModuleGraphRenderData<LFOGraphRenderData<Global>>
{
  int totalSamples = {};
  std::array<int, FFLFOBlockCount> samplesProcessed = {};

  FFLFOProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/) {}
  void DoPostProcess(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoProcessIndicators(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoProcessExchangeState(FBGraphRenderState* /*graphState*/, FBModuleGraphData& /*data*/, bool /*detailGraphs*/, int /*graphIndex*/, int /*exchangeVoice*/, FBModuleProcExchangeStateBase const* /*exchangeState*/) {}
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, bool global, bool /*detailGraphs*/, int graphIndex)
{
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = data->pixelWidth;
  result.staticModuleIndex = static_cast<int>(global ? FFModuleType::GLFO : FFModuleType::VLFO);
  
  int hostSampleCount;
  auto const* state = data->renderState;
  int moduleSlot = state->ModuleProcState()->moduleSlot;
  float bpm = state->ExchangeContainer()->Host()->bpm;
  float hostSampleRate = state->ExchangeContainer()->Host()->sampleRate;
  int moduleType = (int)(global ? FFModuleType::GLFO : FFModuleType::VLFO);
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
LFOGraphRenderData<Global>::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  FFLFOExchangeState const* exchangeFromDSP = nullptr;
  int moduleSlot = moduleProcState->moduleSlot;
  int staticModuleIndex = (int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO);
  int runtimeModuleIndex = moduleProcState->topo->moduleTopoToRuntime.at({ staticModuleIndex, moduleSlot });
  auto const* moduleExchangeState = state->ExchangeContainer()->Modules()[runtimeModuleIndex].get();
  if (exchange)
    if constexpr (Global)
      exchangeFromDSP = &dynamic_cast<FFLFOExchangeState const&>(*moduleExchangeState->Global());
    else
      exchangeFromDSP = &dynamic_cast<FFLFOExchangeState const&>(*moduleExchangeState->Voice()[exchangeVoice]);
  GetProcessor(*moduleProcState).template BeginVoiceOrBlock<Global>(*moduleProcState, exchangeFromDSP, true, detailGraphs, graphIndex, totalSamples);
}

template <bool Global>
FFLFOProcessor&
LFOGraphRenderData<Global>::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *FFSelectDualState<Global>(
    [procState, &state] { return procState->dsp.global.gLFO[state.moduleSlot].processor.get(); },
    [procState, &state] { return procState->dsp.voice[state.voice->slot].vLFO[state.moduleSlot].processor.get(); });
}

template <bool Global>
int 
LFOGraphRenderData<Global>::DoProcess(
  FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GLFO : FFModuleType::VLFO;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } };
  auto type = state->AudioParamList<FFLFOType>(indices, false, -1);
  if (type == FFLFOType::Off)
    return 0;

  if (detailGraphs)
  {
    indices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, graphIndex } };
    auto opType = state->AudioParamList<FFModulationOpType>(indices, exchange, exchangeVoice);
    if (opType == FFModulationOpType::Off)
      return 0;
  }

  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).template Process<Global>(*moduleProcState, true);
}

template <bool Global>
void
FFLFORenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  LFOGraphRenderData<Global> renderData = {};
  auto moduleType = Global ? FFModuleType::GLFO : FFModuleType::VLFO;

  graphData->skipDrawOnEqualsPrimary = false; // need exchange state for all sub-lfos
  graphData->drawMarkersSelector = [](int) { return true; };
  renderData.graphData = graphData;
  renderData.plotParamsSelector = [](auto graphData, bool detailGraphs, int graphIndex) { return PlotParams(graphData, Global, detailGraphs, graphIndex); };
  renderData.totalSamples = PlotParams(graphData, Global, detailGraphs, 0).sampleCount; // just pick one
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gLFO[slot]; };
  renderData.globalMonoOutputSelector = [](void const* procState, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].outputAll; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].vLFO[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].vLFO[slot].outputAll; };

  auto* renderState = graphData->renderState;
  auto* moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBTopoIndices modIndices = { (int)moduleType, moduleSlot };
  FBParamTopoIndices paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } };
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
  auto type = renderState->AudioParamList<FFLFOType>(paramIndices, false, -1);

  int graphCount = detailGraphs ? FFLFOBlockCount : 1;
  for (int i = 0; i < graphCount; i++)
  {
    FBRenderModuleGraph<Global, false>(renderData, detailGraphs, i);
    graphData->graphs[i].moduleSlot = moduleSlot;
    graphData->graphs[i].moduleIndex = (int)moduleType;
    if (detailGraphs)
    {
      FBParamTopoIndices opTypeIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, i } };
      auto opType = renderState->AudioParamList<FFModulationOpType>(opTypeIndices, false, -1);
      graphData->graphs[i].title = FBAsciiToUpper(moduleName + std::string(1, static_cast<char>('A' + i)));
      graphData->graphs[i].defaultSubText = FBAsciiToUpper(FFModulationOpTypeToString(opType));
    }
    else
    {
      graphData->graphs[i].title = moduleName;
      graphData->graphs[i].defaultSubText = FBAsciiToUpper(FFLFOTypeToString(type, Global));
    }
    graphData->graphs[0].ScaleToSelfNormalized();
  }
  if (detailGraphs)
  {
    graphData->ScaleToAllNormalized();
  }
  else
  {
    graphData->graphs[0].ScaleToSelfNormalized();
  }
}

template void FFLFORenderGraph<true>(FBModuleGraphComponentData*, bool);
template void FFLFORenderGraph<false>(FBModuleGraphComponentData*, bool);