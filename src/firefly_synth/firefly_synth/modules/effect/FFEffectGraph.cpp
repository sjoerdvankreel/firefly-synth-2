#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/effect/FFEffectGraph.hpp>
#include <firefly_synth/modules/effect/FFEffectProcessor.hpp>

#include <firefly_base/gui/graph/FBGraphing.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

template <bool Global>
class EffectGraphProcessor final:
public FBModuleGraphProcessor
{
public:
  int totalSamples = {};
  std::array<int, FFEffectBlockCount> samplesProcessed = {};

  FFEffectProcessor& GetProcessor(FBModuleProcState& state);
  EffectGraphProcessor(FBModuleGraphComponentData* componentData) :
  FBModuleGraphProcessor(componentData) {}

  FBModuleGraphPlotParams PlotParams(
    bool detailGraphs, int graphIndex) const override;
  FBSArray<float, FBFixedBlockSamples> const* MonoOutput(
    void const* procState, FBModuleGraphStateParams const& params) override;
  FBModuleProcExchangeStateBase const* ExchangeState(
    void const* exchangeState, FBModuleGraphStateParams const& params) override;

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

template <bool Global>
FBSArray<float, FBFixedBlockSamples> const* 
EffectGraphProcessor<Global>::MonoOutput(
  void const* procState, FBModuleGraphStateParams const& params)
{
  if constexpr (Global)
    return &static_cast<FFProcState const*>(procState)->dsp.global.gEffect[params.moduleSlot].output[0];
  else
    return &static_cast<FFProcState const*>(procState)->dsp.voice[params.voice].vEffect[params.moduleSlot].output[0];
}

template <bool Global>
FBModuleProcExchangeStateBase const* 
EffectGraphProcessor<Global>::ExchangeState(
  void const* exchangeState, FBModuleGraphStateParams const& params)
{
  if constexpr(Global)
    return &static_cast<FFExchangeState const*>(exchangeState)->global.gEffect[params.moduleSlot];
  else
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[params.voice].vEffect[params.moduleSlot];
}

template <bool Global>
FFEffectProcessor&
EffectGraphProcessor<Global>::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *FFSelectDualState<Global>(
    [procState, &state] { return procState->dsp.global.gEffect[state.moduleSlot].processor.get(); },
    [procState, &state] { return procState->dsp.voice[state.voice->slot].vEffect[state.moduleSlot].processor.get(); });
}

template <bool Global>
void
EffectGraphProcessor<Global>::ReleaseOnDemandBuffers(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& /*params*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).ReleaseOnDemandBuffers(
    state->PlugGUI()->HostContext()->Topo(),
    state->ProcContainer());
}

template <bool Global>
FBModuleGraphPlotParams 
EffectGraphProcessor<Global>::PlotParams(
  bool /*detailGraphs*/, int /*graphIndex*/) const
{
  // Need to know SR for graphing because filters.
  // Plot a bit more than exact pixel width to make it look prettier.
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = ComponentData()->pixelWidth * 4;
  result.sampleRate = ComponentData()->pixelWidth * 4.0f / FFEffectPlotLengthSeconds;
  result.staticModuleIndex = static_cast<int>(Global ? FFModuleType::GEffect : FFModuleType::VEffect);
  return result;
}

template <bool Global>
void
EffectGraphProcessor<Global>::BeginVoiceOrBlock(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& params)
{
  samplesProcessed[params.graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).template AllocOnDemandBuffers<Global>(
    state->PlugGUI()->HostContext()->Topo(),
    state->ProcContainer(), moduleProcState->moduleSlot,
    true, moduleProcState->input->sampleRate);
  GetProcessor(*moduleProcState).template BeginVoiceOrBlock<Global>(
    *moduleProcState, true, params.detailGraphs, params.graphIndex, totalSamples);
}

template <bool Global>
void
EffectGraphProcessor<Global>::PostProcess(
  FBGraphRenderState* state, FBModuleGraphData& /*data*/,
  FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points)
{
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, params.graphIndex } };
  auto kind = state->AudioParamList<FFEffectKind>(indices, params.exchange, params.exchangeVoice);

  points.bipolar = !params.detailGraphs || FFEffectKindIsShaper(kind) || kind == FFEffectKind::Compressor;
  points.plotLogStart = 20.0f;
  points.plotLogEnd = 20000.0f;
  points.plotLogarithmic = !points.bipolar;
  points.roundPathCorners = !points.bipolar;

  if (!params.detailGraphs)
    return;
  if (!FFEffectKindIsFilter(kind))
    return;
  if (points.l.size() == 0)
    return;

  state->FFT(points.l);
}

template <bool Global>
void
EffectGraphProcessor<Global>::ProcessExchangeState(
  FBGraphRenderState* graphState, FBModuleGraphData& data, 
  FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState)
{
  auto effectExchange = dynamic_cast<FFEffectExchangeState const*>(exchangeState);
  if (!params.detailGraphs)
  {
    data.exchangeGainValue = std::max(data.exchangeGainValue, effectExchange->output);
    return;
  }

  int moduleSlot = graphState->ModuleProcState()->moduleSlot;
  data.exchangeGainValue = std::max(data.exchangeGainValue, effectExchange->outputs[params.graphIndex]);
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, params.graphIndex } };
  auto kind = graphState->AudioParamList<FFEffectKind>(indices, false, -1);
  if (FFEffectKindIsSVF(kind))
    data.exchangeMainText = FBToStringHz(effectExchange->stVarFreqs[params.graphIndex], 2);
  else if (FFEffectKindIsShaper(kind))
    data.exchangeMainText = FBToStringPercent(effectExchange->shaperDrives[params.graphIndex], 2) + " Drive";
  else if (kind == FFEffectKind::CombPlus)
    data.exchangeMainText = FBToStringHz(effectExchange->combPlusFreqs[params.graphIndex], 2);
  else if (kind == FFEffectKind::CombMin)
    data.exchangeMainText = FBToStringHz(effectExchange->combMinFreqs[params.graphIndex], 2);
  else if (kind == FFEffectKind::Comb)
    data.exchangeMainText = FBFormatDoubleCLocale(effectExchange->combPlusFreqs[params.graphIndex], 2) + " / " +
      FBToStringHz(effectExchange->combMinFreqs[params.graphIndex], 2);
  else if (kind == FFEffectKind::Compressor)
  {
    // TODO
  }
  else
    FB_ASSERT(kind == FFEffectKind::Off);
}

template <bool Global>
int 
EffectGraphProcessor<Global>::Process(
  FBGraphRenderState* state, FBModuleGraphProcessParams const& params)
{
  bool plotSpecificFilter = false;
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::On, 0 } };
  bool on = state->AudioParamBool(indices, false, -1);
  if (!on)
    return 0;

  if (params.detailGraphs)
  {
    indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, params.graphIndex } };
    auto kind = state->AudioParamList<FFEffectKind>(indices, params.exchange, params.exchangeVoice);
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
        input[c].Set(s, (samplesProcessed[params.graphIndex] + s) == 0 ? 1.0f : 0.0f);
      else
        input[c].Set(s, ((samplesProcessed[params.graphIndex] + s) / static_cast<float>(totalSamples)) * 2.0f - 1.0f);
  
  auto const* exchangeFromDSP = GetEffectExchangeStateFromDSP(state, Global, moduleSlot, params.exchange, params.exchangeVoice);
  samplesProcessed[params.graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).template Process<Global>(*moduleProcState, exchangeFromDSP, true);
}

template <bool Global>
void
FFEffectRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  EffectGraphProcessor<Global> processor(graphData);
  auto moduleType = Global ? FFModuleType::GEffect : FFModuleType::VEffect;
  processor.totalSamples = processor.PlotParams(Global, -1).sampleCount;

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
    FBRenderModuleGraph(&processor, Global, false, detailGraphs, i);
    graphData->graphs[i].moduleSlot = moduleSlot;
    graphData->graphs[i].moduleIndex = (int)moduleType;
    graphData->graphs[i].displayGainAsDb = true;

    FBParamTopoIndices indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, i } };
    auto kind = renderState->AudioParamList<FFEffectKind>(indices, false, -1);
    if(!detailGraphs || kind != FFEffectKind::Compressor)
      graphData->graphs[i].ScaleToSelfNormalized();

    if (detailGraphs)
    {
      indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FilterMode, i } };
      auto filterMode = renderState->AudioParamList<FFEffectFilterMode>(indices, false, -1);
      indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistDrive, i } };
      auto distDrive = renderState->AudioParamLinear(indices, false, -1);
      graphData->graphs[i].title = std::string(1, static_cast<char>('A' + i));
      graphData->graphs[i].title += ": " + FFEffectKindToString(kind);

      if (FFEffectKindIsSVF(kind))
      {
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarFreqFreq, i } };
        float freq = renderState->AudioParamLog2(indices, false, -1);
        indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarPitchCoarse, i } };
        float coarse = renderState->AudioParamLinear(indices, false, -1);
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
      indices = { { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Oversample, 0 } };
      bool oversample = renderState->AudioParamBool(indices, false, -1);
      graphData->graphs[i].title = moduleName + ": " + (on ? "On" : "Off");
      if (on && oversample)
        graphData->graphs[i].title += ", Oversample";
    }    
  }
}

template void FFEffectRenderGraph<true>(FBModuleGraphComponentData*, bool);
template void FFEffectRenderGraph<false>(FBModuleGraphComponentData*, bool);