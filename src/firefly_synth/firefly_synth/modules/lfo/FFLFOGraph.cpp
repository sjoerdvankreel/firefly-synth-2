#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/lfo/FFLFOGraph.hpp>
#include <firefly_synth/modules/lfo/FFLFOProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

template <bool Global>
struct LFOGraphRenderData final:
public FBModuleGraphRenderData<LFOGraphRenderData<Global>>
{
  int totalSamples = {};
  std::array<int, FFLFOBlockCount + 1> samplesProcessed = {};

  FFLFOProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoPostProcess(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, bool global, int graphIndex)
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
  int snapToLFO = graphIndex == FFLFOBlockCount ? 0 : graphIndex;
  FBParamTopoIndices indices = { { moduleType, moduleSlot }, { (int)FFLFOParam::Sync, 0 } };
  bool sync = state->AudioParamBool(indices, false, -1);
  if (sync)
  {
    indices = { { moduleType, moduleSlot }, { (int)FFLFOParam::RateBars, snapToLFO } };
    hostSampleCount = state->AudioParamBarsSamples(indices, false, -1, hostSampleRate, bpm);
  }
  else
  {
    indices = { { moduleType, moduleSlot }, { (int)FFLFOParam::RateHz, snapToLFO } };
    hostSampleCount = state->AudioParamLinearFreqSamples(indices, false, -1, hostSampleRate);
  }
  result.sampleRate = result.sampleCount * hostSampleRate / hostSampleCount;
  return result;
}

template <bool Global>
void 
LFOGraphRenderData<Global>::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  FFLFOExchangeState const* exchangeState = nullptr;
  int moduleSlot = moduleProcState->moduleSlot;
  int staticModuleIndex = (int)(Global ? FFModuleType::GLFO : FFModuleType::VLFO);
  int runtimeModuleIndex = moduleProcState->topo->moduleTopoToRuntime.at({ staticModuleIndex, moduleSlot });
  auto const* moduleExchangeState = state->ExchangeContainer()->Modules()[runtimeModuleIndex].get();
  if (exchange)
    if constexpr (Global)
      exchangeState = &dynamic_cast<FFLFOExchangeState const&>(*moduleExchangeState->Global());
    else
      exchangeState = &dynamic_cast<FFLFOExchangeState const&>(*moduleExchangeState->Voice()[exchangeVoice]);
  GetProcessor(*moduleProcState).template BeginVoiceOrBlock<Global>(true, graphIndex, totalSamples, exchangeState, *moduleProcState);
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
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GLFO : FFModuleType::VLFO;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } };
  auto type = state->AudioParamList<FFLFOType>(indices, false, -1);
  if (type == FFLFOType::Off)
    return 0;

  if (graphIndex != FFLFOBlockCount)
  {
    indices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, graphIndex } };
    auto opType = state->AudioParamList<FFLFOOpType>(indices, exchange, exchangeVoice);
    if (opType == FFLFOOpType::Off)
      return 0;
  }

  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).template Process<Global>(*moduleProcState);
}

template <bool Global>
void
FFLFORenderGraph(FBModuleGraphComponentData* graphData)
{
  LFOGraphRenderData<Global> renderData = {};
  auto moduleType = Global ? FFModuleType::GLFO : FFModuleType::VLFO;

  graphData->bipolar = false;
  graphData->drawClipBoundaries = false;
  graphData->skipDrawOnEqualsPrimary = false; // need exchange state for all sub-lfos
  graphData->drawMarkersSelector = [](int) { return true; };
  renderData.graphData = graphData;
  renderData.plotParamsSelector = [](auto graphData, int graphIndex) { return PlotParams(graphData, Global, graphIndex); };
  renderData.totalSamples = PlotParams(graphData, Global, 0).sampleCount; // just pick one
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gLFO[slot]; };
  renderData.globalMonoOutputSelector = [](void const* procState, int slot, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gLFO[slot].output; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].vLFO[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].vLFO[slot].output; };

  auto* renderState = graphData->renderState;
  auto* moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBTopoIndices modIndices = { (int)moduleType, moduleSlot };
  FBParamTopoIndices paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } };
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->graphName;
  auto type = renderState->AudioParamList<FFLFOType>(paramIndices, false, -1);
  for (int i = 0; i <= FFLFOBlockCount; i++)
  {
    FBRenderModuleGraph<Global, false>(renderData, i);
    if (i == FFLFOBlockCount)
      graphData->graphs[i].text = moduleName + (type != FFLFOType::Off? "ALL": "ALL OFF");
    else
    {
      FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, i } };
      auto opType = renderState->AudioParamList<FFLFOOpType>(indices, false, -1);
      bool blockOn = type != FFLFOType::Off && opType != FFLFOOpType::Off;
      graphData->graphs[i].text = moduleName + std::string(1, static_cast<char>('A' + i));
      if (!blockOn)
        graphData->graphs[i].text += " OFF";
    }
  }
}

template void FFLFORenderGraph<true>(FBModuleGraphComponentData* graphData);
template void FFLFORenderGraph<false>(FBModuleGraphComponentData* graphData);