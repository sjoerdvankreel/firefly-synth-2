#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGraph.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessorProcess.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

struct GlobalUniGraphRenderData final :
public FBModuleGraphRenderData<GlobalUniGraphRenderData>
{
  int totalSamples = {};
  std::array<int, (int)FFGlobalUniTarget::Count> samplesProcessed = {};

  FFGlobalUniProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoiceOrBlock(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoPostProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoProcessIndicators(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
  void DoReleaseOnDemandBuffers(FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/) {}
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data, int /*graphIndex*/)
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.sampleCount = data->pixelWidth * 2;
  result.staticModuleIndex = (int)FFModuleType::GlobalUni;
  return result;
}

FFGlobalUniProcessor&
GlobalUniGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.global.globalUni.processor;
}

void
GlobalUniGraphRenderData::DoBeginVoiceOrBlock(
  FBGraphRenderState* state, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).BeginBlock(*moduleProcState);
}

int
GlobalUniGraphRenderData::DoProcess(
  FBGraphRenderState* /*state*/, int graphIndex, bool /*exchange*/, int /*exchangeVoice*/)
{
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return std::clamp(totalSamples - samplesProcessed[graphIndex], 0, FBFixedBlockSamples);
}

void
GlobalUniGraphRenderData::DoProcessIndicators(
  FBGraphRenderState* state,
  int graphIndex, bool exchange,
  int /*exchangeVoice*/, FBModuleGraphPoints& points)
{
  int voiceCount = state->AudioParamDiscrete({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } }, exchange, -1);
  auto mode = state->AudioParamList<FFGlobalUniMode>({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::Mode, graphIndex } }, exchange, -1);
  if (mode == FFGlobalUniMode::Auto)
    for (int i = 0; i < voiceCount; i++)
      points.pointIndicators.push_back((int)(i / (voiceCount - 1.0f) * points.l.size()));
  if (mode == FFGlobalUniMode::Manual)
    for (int i = 0; i < voiceCount; i++)
      points.pointIndicators.push_back((int)(i / (voiceCount - 1.0f) * 0.5f * points.l.size()));
}

void 
GlobalUniGraphRenderData::DoPostProcess(
  FBGraphRenderState* /*state*/, int /*graphIndex*/,
  bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& points)
{
  // This isn't exactly the most efficient way to draw an arc,
  // but i didnt feel like breaking into the base painting code.
  for (int i = 0; i < points.l.size(); i++)
  {
    float a = i / (float)points.l.size();
    float y = std::sin(a * FBPi);
    points.l[i] = y; 
  }
}

void
FFGlobalUniRenderGraph(FBModuleGraphComponentData* graphData)
{
  GlobalUniGraphRenderData renderData = {};
  graphData->bipolar = false;
  graphData->drawClipBoundaries = false;
  graphData->skipDrawOnEqualsPrimary = true;
  graphData->drawMarkersSelector = [](int) { return true; };
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.totalSamples = PlotParams(graphData, 0).sampleCount;
  renderData.globalExchangeSelector = [](void const* exchangeState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.globalUni[0]; };
  renderData.globalMonoOutputSelector = [](void const* procState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.globalUni.fakeGraphOutput; };

  for(int i = 0; i < (int)FFGlobalUniTarget::Count; i++)
    FBRenderModuleGraph<true, false>(renderData, i);
}