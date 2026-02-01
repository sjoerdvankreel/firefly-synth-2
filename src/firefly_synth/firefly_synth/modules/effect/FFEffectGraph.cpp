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
  void DoPostProcess(FBGraphRenderState* state, FBModuleGraphData& data, bool detailGraphs, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoProcessIndicators(FBGraphRenderState* /*state*/, bool /*detailGraphs*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& /*points*/) {}
  void DoProcessExchangeState(FBGraphRenderState* graphState, FBModuleGraphData& data, bool detailGraphs, int graphIndex, int exchangeVoice, FBModuleProcExchangeStateBase const* exchangeState);
};

static std::string
FilterMainText(FFEffectFilterMode mode, float freq, float coarse)
{
  switch (mode)
  {
  case FFEffectFilterMode::Freq: return FBToStringHz(freq, 2);
  case FFEffectFilterMode::Pitch: return FBPitchToStringNotes(coarse);
  case FFEffectFilterMode::Track: return FBPitchToStringSemis(coarse, 0.0f, 2, true);
  default: FB_ASSERT(false); return {};
  }
}

static std::string
CombMainText(FFEffectFilterMode mode, float freqPlus, float coarsePlus, float freqMin, float coarseMin)
{
  switch (mode)
  {
  case FFEffectFilterMode::Freq: return FBFormatDoubleCLocale(freqPlus, 2) + " / " + FBToStringHz(freqMin, 2);
  case FFEffectFilterMode::Pitch: return FBPitchToStringNotes(coarsePlus) + " / " + FBPitchToStringNotes(coarseMin);
  case FFEffectFilterMode::Track: return FBPitchToStringSemis(coarsePlus, 0.0f, 2, false) + " / " + FBPitchToStringSemis(coarseMin, 0.0f, 2, true);
  default: FB_ASSERT(false); return {};
  }
}

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
EffectGraphRenderData<Global>::DoProcessExchangeState(
  FBGraphRenderState* graphState, FBModuleGraphData& data,
  bool detailGraphs, int graphIndex, int /*exchangeVoice*/,
  FBModuleProcExchangeStateBase const* exchangeState)
{
  auto effectExchange = dynamic_cast<FFEffectExchangeState const*>(exchangeState);
  if (!detailGraphs)
  {
    data.exchangeGainValue = effectExchange->output;
    return;
  }

  data.exchangeGainValue = effectExchange->outputs[graphIndex];
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, 0 }, { (int)FFEffectParam::Kind, graphIndex } };
  auto kind = graphState->AudioParamList<FFEffectKind>(indices, false, -1);
  if (kind == FFEffectKind::StVar)
    data.exchangeMainText = FBToStringHz(effectExchange->stVarFreqs[graphIndex], 2);
  else if (FFEffectKindIsShaper(kind))
    data.exchangeMainText = FBToStringPercent(effectExchange->shaperDrives[graphIndex], 2) + " Drive";
  else if (kind == FFEffectKind::CombPlus)
    data.exchangeMainText = FBToStringHz(effectExchange->combPlusFreqs[graphIndex], 2);
  else if (kind == FFEffectKind::CombMin)
    data.exchangeMainText = FBToStringHz(effectExchange->combMinFreqs[graphIndex], 2);
  else if (kind == FFEffectKind::Comb)
    data.exchangeMainText = FBFormatDoubleCLocale(effectExchange->combPlusFreqs[graphIndex], 2) + " / " + 
      FBToStringHz(effectExchange->combMinFreqs[graphIndex], 2);
  else
    FB_ASSERT(false);
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
  FBGraphRenderState* state, FBModuleGraphData& /*data*/,
  bool detailGraphs, int graphIndex,
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
  if (!FFEffectKindIsFilter(kind))
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
    plotSpecificFilter = FFEffectKindIsFilter(kind);
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
    graphData->graphs[i].displayGainAsDb = true;
    graphData->graphs[i].ScaleToSelfNormalized();

    if (detailGraphs)
    {
      FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, i } };
      auto kind = renderState->AudioParamList<FFEffectKind>(indices, false, -1);
      indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FilterMode, i } };
      auto filterMode = renderState->AudioParamList<FFEffectFilterMode>(indices, false, -1);
      indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistDrive, i } };
      auto distDrive = renderState->AudioParamLinear(indices, false, -1);
      graphData->graphs[i].title = FBAsciiToUpper(moduleName + std::string(1, static_cast<char>('A' + i)));
      graphData->graphs[i].title += ": " + FFEffectKindToString(kind);

      if (kind == FFEffectKind::StVar)
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarMode, i } };
        auto stVarMode = renderState->AudioParamList<FFStateVariableFilterMode>(indices, false, -1);
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarFreqFreq, i } };
        float freq = renderState->AudioParamLog2(indices, false, -1);
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarPitchCoarse, i } };
        float coarse = renderState->AudioParamLinear(indices, false, -1);
        graphData->graphs[i].title += ", " + FFStateVariableFilterModeToString(stVarMode);
        graphData->graphs[i].defaultMainText = FilterMainText(filterMode, freq, coarse);
      }
      if(FFEffectKindIsComb(kind))
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqFreqPlus, i } };
        float freqPlus = renderState->AudioParamLog2(indices, false, -1);
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombPitchCoarsePlus, i } };
        float coarsePlus = renderState->AudioParamLinear(indices, false, -1);
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqFreqMin, i } };
        float freqMin = renderState->AudioParamLog2(indices, false, -1);
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombPitchCoarseMin, i } };
        float coarseMin = renderState->AudioParamLinear(indices, false, -1);
        if(kind == FFEffectKind::CombPlus)
          graphData->graphs[i].defaultMainText = FilterMainText(filterMode, freqPlus, coarsePlus);
        else if (kind == FFEffectKind::CombMin)
          graphData->graphs[i].defaultMainText = FilterMainText(filterMode, freqMin, coarseMin);
        else
          graphData->graphs[i].defaultMainText = CombMainText(filterMode, freqPlus, coarsePlus, freqMin, coarseMin);
      }

      if (FFEffectKindIsFilter(kind))
      {
        graphData->graphs[i].title += ", " + FFEffectFilterModeToString(filterMode);
      }

      if (FFEffectKindIsShaper(kind))
      {
        graphData->graphs[i].defaultMainText = FBToStringPercent(distDrive, 2) + " Drive";
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
    }
    else
    {
      FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Oversample, 0 } };
      bool oversample = renderState->AudioParamBool(indices, false, -1);
      graphData->graphs[i].title = moduleName + ": " + (on ? "On" : "Off");
      if (on && oversample)
        graphData->graphs[i].title += ", Oversample";
    }    
  }
}

template void FFEffectRenderGraph<true>(FBModuleGraphComponentData*, bool);
template void FFEffectRenderGraph<false>(FBModuleGraphComponentData*, bool);