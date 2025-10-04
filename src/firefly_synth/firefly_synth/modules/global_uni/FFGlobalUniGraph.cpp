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
  int samplesProcessed = {};

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
  samplesProcessed = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).BeginBlock(*moduleProcState);
}

int
GlobalUniGraphRenderData::DoProcess(
  FBGraphRenderState* /*state*/, int /*graphIndex*/, bool /*exchange*/, int /*exchangeVoice*/)
{
  samplesProcessed += FBFixedBlockSamples;
  return std::clamp(totalSamples - samplesProcessed, 0, FBFixedBlockSamples);
}

void
GlobalUniGraphRenderData::DoProcessIndicators(
  FBGraphRenderState* state,
  int /*graphIndex*/, bool exchange,
  int /*exchangeVoice*/, FBModuleGraphPoints& points)
{
  int slot = graphData->fixedGraphIndex;
  FBSArray<float, FBFixedBlockSamples> defaultBlock;
  FFProcDSPState* procState = &state->ModuleProcState()->ProcAs<FFProcState>()->dsp;
  FFGlobalUniProcessor* processor = procState->global.globalUni.processor.get();
  float targetDefault = processor->GetTargetDefault((FFGlobalUniTarget)slot);
  int voiceCount = state->AudioParamDiscrete({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } }, exchange, -1);
  for (int i = 0; i < voiceCount; i++)
  {
    defaultBlock.Fill(FBBatch<float>(targetDefault));
    processor->ApplyToVoice(*state->ModuleProcState(), (FFGlobalUniTarget)slot, i, defaultBlock);
    points.pointIndicators.push_back((int)(defaultBlock.Get(0) * points.l.size()));
  }
}

void 
GlobalUniGraphRenderData::DoPostProcess(
  FBGraphRenderState* state, int /*graphIndex*/,
  bool /*exchange*/, int /*exchangeVoice*/, FBModuleGraphPoints& points)
{
  int slot = graphData->fixedGraphIndex;
  FFProcDSPState* procState = &state->ModuleProcState()->ProcAs<FFProcState>()->dsp;
  FFGlobalUniProcessor* processor = procState->global.globalUni.processor.get();
  if (processor->GetTargetDefault((FFGlobalUniTarget)slot) != 0.5f)
  {
    for (int i = 0; i < points.l.size(); i++)
      points.l[i] = i / (float)points.l.size();
    return;
  }

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
  graphData->pointIndicatorSize = 4;
  graphData->drawClipBoundaries = false;
  graphData->fillPointIndicators = true;
  graphData->skipDrawOnEqualsPrimary = true;
  graphData->drawMarkersSelector = [](int) { return false; };
  graphData->renderState->ModuleProcState()->moduleSlot = 0;
  graphData->paintAsDisabled = graphData->renderState->AudioParamList<FFGlobalUniMode>(
    { { (int)FFModuleType::GlobalUni, 0 },
    { (int)FFGlobalUniParam::Mode, graphData->fixedGraphIndex } }, false, -1) == FFGlobalUniMode::Off;

  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.totalSamples = PlotParams(graphData, 0).sampleCount;
  renderData.globalExchangeSelector = [](void const* exchangeState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFExchangeState const*>(exchangeState)->global.globalUni[0]; };
  renderData.globalMonoOutputSelector = [](void const* procState, int /*slot*/, int /*graphIndex*/) {
    return &static_cast<FFProcState const*>(procState)->dsp.global.globalUni.fakeGraphOutput; };
  FBRenderModuleGraph<true, false>(renderData, 0);
}