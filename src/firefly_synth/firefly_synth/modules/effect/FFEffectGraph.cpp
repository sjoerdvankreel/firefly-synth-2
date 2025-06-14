#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/effect/FFEffectGraph.hpp>
#include <firefly_synth/modules/effect/FFEffectProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

template <bool Global>
struct EffectGraphRenderData final:
public FBModuleGraphRenderData<EffectGraphRenderData<Global>>
{
  int totalSamples = {};
  std::array<int, FFEffectBlockCount + 1> samplesProcessed = {};

  FFEffectProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoPostProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, bool global, int /*graphIndex*/)
{
  // Need to know SR for graphing because filters.
  // Plot a bit more than exact pixel width to make it look prettier.
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = data->pixelWidth * 4;
  result.sampleRate = data->pixelWidth * 4.0f / FFEffectPlotLengthSeconds;
  result.staticModuleIndex = static_cast<int>(global ? FFModuleType::GEffect : FFModuleType::VEffect);
  return result;
}

template <bool Global>
void 
EffectGraphRenderData<Global>::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).InitializeBuffers(true, moduleProcState->input->sampleRate);
  GetProcessor(*moduleProcState).template BeginVoiceOrBlock<Global>(true, graphIndex, totalSamples, *moduleProcState);
}

template <bool Global>
FFEffectProcessor&
EffectGraphRenderData<Global>::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *FFSelectDualState<Global>(
    [procState, &state] { return procState->dsp.global.gEffect[state.moduleSlot].processor.get(); },
    [procState, &state] { return procState->dsp.voice[state.voice->slot].vEffect[state.moduleSlot].processor.get(); });
}

template <bool Global>
void
EffectGraphRenderData<Global>::DoPostProcess(
  FBGraphRenderState* state, int graphIndex,
  bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
{
  if (graphIndex == FFEffectBlockCount)
    return;

  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, graphIndex } };
  auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);
  if (kind != FFEffectKind::StVar && kind != FFEffectKind::Comb && kind != FFEffectKind::CombPlus && kind != FFEffectKind::CombMin)
    return;
  if (points.l.size() == 0)
    return;
  state->FFT(points.l);
  for (int i = 0; i < points.l.size(); i++)
    points.l[i] = FBToBipolar(points.l[i]);
}

template <bool Global>
int 
EffectGraphRenderData<Global>::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  bool plotSpecificFilter = false;
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::On, 0 } };
  bool on = state->AudioParamBool(indices, false, -1);
  if (!on)
    return 0;

  if (graphIndex != FFEffectBlockCount)
  {
    indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, graphIndex } };
    auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);
    plotSpecificFilter = kind == FFEffectKind::StVar || kind == FFEffectKind::Comb || kind == FFEffectKind::CombPlus || kind == FFEffectKind::CombMin;
    if (kind == FFEffectKind::Off)
      return 0;
  }

  auto* procState = moduleProcState->ProcAs<FFProcState>();
  auto& input = *FFSelectDualState<Global>(
    [procState, moduleProcState]() { return &procState->dsp.global.gEffect[moduleProcState->moduleSlot].input; },
    [procState, moduleProcState]() { return &procState->dsp.voice[moduleProcState->voice->slot].vEffect[moduleProcState->moduleSlot].input; });
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      if (plotSpecificFilter)
        input[c].Set(s, (samplesProcessed[graphIndex] + s) == 0 ? 1.0f : 0.0f);
      else
        input[c].Set(s, ((samplesProcessed[graphIndex] + s) / static_cast<float>(totalSamples)) * 2.0f - 1.0f);
  
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).template Process<Global>(*moduleProcState);
}

template <bool Global>
void
FFEffectRenderGraph(FBModuleGraphComponentData* graphData)
{
  EffectGraphRenderData<Global> renderData = {};
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;

  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  renderData.graphData = graphData;
  renderData.plotParamsSelector = [](auto graphData, int graphIndex) { return PlotParams(graphData, Global, graphIndex); };
  renderData.totalSamples = PlotParams(graphData, Global, -1).sampleCount;
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gEffect[slot]; };
  renderData.globalMonoOutputSelector = [](void const* procState, int slot, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gEffect[slot].output[0]; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].vEffect[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].vEffect[slot].output[0]; };

  auto* renderState = graphData->renderState;
  auto* moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBTopoIndices modIndices = { (int)moduleType, moduleSlot };
  FBParamTopoIndices paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::On, 0 } };
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->graphName;
  bool on = renderState->AudioParamBool(paramIndices, false, -1);
  for (int i = 0; i <= FFEffectBlockCount; i++)
  {
    FBRenderModuleGraph<Global, false>(renderData, i);
    if (i == FFEffectBlockCount)
      graphData->graphs[i].text = moduleName + (on? "ALL": "ALL OFF");
    else
    {
      FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, i } };
      auto kind = renderState->AudioParamList<FFEffectKind>(indices, false, -1);
      bool blockOn = on && kind != FFEffectKind::Off;
      graphData->graphs[i].text = moduleName + std::string(1, static_cast<char>('A' + i));
      if (!blockOn)
        graphData->graphs[i].text += " OFF";
    }
  }
}

template void FFEffectRenderGraph<true>(FBModuleGraphComponentData* graphData);
template void FFEffectRenderGraph<false>(FBModuleGraphComponentData* graphData);