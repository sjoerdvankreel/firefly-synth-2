#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/effect/FFEffectGraph.hpp>
#include <firefly_synth/modules/effect/FFEffectProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

template <bool Global>
struct EffectGraphRenderData final:
public FBModuleGraphRenderData<EffectGraphRenderData<Global>>
{
  int totalSamples = {};
  std::array<int, FFEffectBlockCount> samplesProcessed = {};

  FFEffectProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice);
  void DoPostProcess(FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoProcessIndicators(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
};

static FFEffectExchangeState const*
GetEffectExchangeStateFromDSP(FBGraphRenderState* state, bool global, int slot, bool exchange, int exchangeVoice)
{
  auto* moduleProcState = state->ModuleProcState();
  FFEffectExchangeState const* exchangeFromDSP = nullptr;
  auto moduleType = global ? FFModuleType::GEffect : FFModuleType::VEffect;
  int runtimeModuleIndex = moduleProcState->topo->moduleTopoToRuntime.at({ (int)moduleType, slot });
  auto const* moduleExchangeState = state->ExchangeContainer()->Modules()[runtimeModuleIndex].get();
  if (exchange)
    if(global)
      exchangeFromDSP = &dynamic_cast<FFEffectExchangeState const&>(*moduleExchangeState->Global());
    else
      exchangeFromDSP = &dynamic_cast<FFEffectExchangeState const&>(*moduleExchangeState->Voice()[exchangeVoice]);
  return exchangeFromDSP;
}

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
EffectGraphRenderData<Global>::DoReleaseOnDemandBuffers(
  FBGraphRenderState* state, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).ReleaseOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(),
    state->ProcContainer());
}

template <bool Global>
void 
EffectGraphRenderData<Global>::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).template AllocOnDemandBuffers<Global>(
    state->PlugGUI()->HostContext()->Topo(), 
    state->ProcContainer(), moduleProcState->moduleSlot, 
    true, moduleProcState->input->sampleRate);
  GetProcessor(*moduleProcState).template BeginVoiceOrBlock<Global>(
    *moduleProcState, true, detailGraphs, graphIndex, totalSamples);
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
  FBGraphRenderState* state, bool detailGraphs, int graphIndex,
  bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
{
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, graphIndex } };
  auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);

  points.bipolar = !detailGraphs || (kind == FFEffectKind::Clip || kind == FFEffectKind::Fold || kind == FFEffectKind::Skew);
  points.plotLogStart = 20.0f;
  points.plotLogEnd = 20000.0f;
  points.plotLogarithmic = !points.bipolar;
  points.roundPathCorners = !points.bipolar;

  if (!detailGraphs)
    return;

  if (kind != FFEffectKind::StVar && kind != FFEffectKind::Comb &&
    kind != FFEffectKind::CombPlus && kind != FFEffectKind::CombMin)
    return;
  if (points.l.size() == 0)
    return;
  state->FFT(points.l);
}

template <bool Global>
int 
EffectGraphRenderData<Global>::DoProcess(
  FBGraphRenderState* state, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice)
{
  bool plotSpecificFilter = false;
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::On, 0 } };
  bool on = state->AudioParamBool(indices, false, -1);
  if (!on)
    return 0;

  if (detailGraphs)
  {
    indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, graphIndex } };
    auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);
    plotSpecificFilter = kind == FFEffectKind::StVar || kind == FFEffectKind::Comb ||
      kind == FFEffectKind::CombMin || kind == FFEffectKind::CombPlus;
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
  
  auto const* exchangeFromDSP = GetEffectExchangeStateFromDSP(state, Global, moduleSlot, exchange, exchangeVoice);
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).template Process<Global>(*moduleProcState, exchangeFromDSP, true);
}

template <bool Global>
void
FFEffectRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  EffectGraphRenderData<Global> renderData = {};
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;

  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  renderData.graphData = graphData;
  renderData.plotParamsSelector = [](auto graphData, bool, int graphIndex) { return PlotParams(graphData, Global, graphIndex); };
  renderData.totalSamples = PlotParams(graphData, Global, -1).sampleCount;
  renderData.globalExchangeSelector = [](void const* exchangeState, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gEffect[slot]; };
  renderData.globalMonoOutputSelector = [](void const* procState, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.gEffect[slot].output[0]; };
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].vEffect[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot, bool /*detailGraphs*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].vEffect[slot].output[0]; };

  auto* renderState = graphData->renderState;
  auto* moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBTopoIndices modIndices = { (int)moduleType, moduleSlot };
  FBParamTopoIndices paramIndices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::On, 0 } };
  auto moduleName = graphData->renderState->ModuleProcState()->topo->ModuleAtTopo(modIndices)->name;
  bool on = renderState->AudioParamBool(paramIndices, false, -1);

  int graphCount = detailGraphs ? FFEffectBlockCount : 1;
  for (int i = 0; i < graphCount; i++)
  {
    FBRenderModuleGraph<Global, false>(renderData, detailGraphs, i);
    graphData->graphs[i].moduleSlot = moduleSlot;
    graphData->graphs[i].moduleIndex = (int)moduleType;
    if (detailGraphs)
    {
      FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, i } };
      auto kind = renderState->AudioParamList<FFEffectKind>(indices, false, -1);
      graphData->graphs[i].title = FBAsciiToUpper(moduleName + std::string(1, static_cast<char>('A' + i)));
      graphData->graphs[i].title += ": " + FFEffectKindToString(kind);
      if (kind == FFEffectKind::StVar)
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarMode, i } };
        auto mode = renderState->AudioParamList<FFStateVariableFilterMode>(indices, false, -1);
        graphData->graphs[i].title += ", " + FFStateVariableFilterModeToString(mode);
      }
      if (kind == FFEffectKind::StVar || kind == FFEffectKind::Comb || kind == FFEffectKind::CombPlus || kind == FFEffectKind::CombMin)
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FilterMode, i } };
        auto mode = renderState->AudioParamList<FFEffectFilterMode>(indices, false, -1);
        graphData->graphs[i].title += ", " + FFEffectFilterModeToString(mode);
      }
      if (kind == FFEffectKind::Clip)
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::ClipMode, i } };
        auto mode = renderState->AudioParamList<FFEffectClipMode>(indices, false, -1);
        graphData->graphs[i].title += ", " + FFEffectClipModeToString(mode);
      }
      if (kind == FFEffectKind::Fold)
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FoldMode, i } };
        auto mode = renderState->AudioParamList<FFEffectFoldMode>(indices, false, -1);
        graphData->graphs[i].title += ", " + FFEffectFoldModeToString(mode);
      }
      if (kind == FFEffectKind::Skew)
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::SkewMode, i } };
        auto mode = renderState->AudioParamList<FFEffectSkewMode>(indices, false, -1);
        graphData->graphs[i].title += ", " + FFEffectSkewModeToString(mode);
      }


      //graphData->graphs[i].subtext = FBAsciiToUpper(FFEffectKindToString(kind)); // todo
    }
    else
    {
      FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Oversample, 0 } };
      bool oversample = renderState->AudioParamBool(indices, false, -1);
      graphData->graphs[i].title = moduleName + ": " + (on ? "On" : "Off");
      if (on && oversample)
        graphData->graphs[i].title += ", Oversample";

      //graphData->graphs[i].subtext = on ? "On" : "Off"; // todo
    }
    graphData->graphs[i].ScaleToSelfNormalized();
  }
}

template void FFEffectRenderGraph<true>(FBModuleGraphComponentData*, bool);
template void FFEffectRenderGraph<false>(FBModuleGraphComponentData*, bool);