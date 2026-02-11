#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGraph.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessor.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

class GlobalUniGraphProcessor final :
public FBModuleGraphProcessor
{
public:
  int totalSamples = {};
  int samplesProcessed = {};

  FFGlobalUniProcessor& GetProcessor(FBModuleProcState& state);
  GlobalUniGraphProcessor(FBModuleGraphComponentData* componentData) :
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
    FBModuleGraphProcessParams const& /*params*/) override {}
  void ProcessIndicators(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) override;
  void PostProcess(FBGraphRenderState* state,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) override;
  void ProcessExchangeState(FBGraphRenderState* /*graphState*/,
    FBModuleGraphData& /*data*/, FBModuleGraphProcessParams const& /*params*/, FBModuleProcExchangeStateBase const* /*exchangeState*/) override {}
};

FFGlobalUniProcessor&
GlobalUniGraphProcessor::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  return *procState->dsp.global.globalUni.processor;
}

FBSArray<float, FBFixedBlockSamples> const* 
GlobalUniGraphProcessor::MonoOutput(
  void const* procState, FBModuleGraphStateParams const& /*params*/)
{
  return &static_cast<FFProcState const*>(procState)->dsp.global.globalUni.fakeGraphOutput;
}

FBModuleProcExchangeStateBase const* 
GlobalUniGraphProcessor::ExchangeState(
  void const* exchangeState, FBModuleGraphStateParams const& /*params*/)
{
  return &static_cast<FFExchangeState const*>(exchangeState)->global.globalUni[0];
}

int
GlobalUniGraphProcessor::Process(
  FBGraphRenderState* /*state*/,
  FBModuleGraphProcessParams const& /*params*/)
{
  samplesProcessed += FBFixedBlockSamples;
  return std::clamp(totalSamples - samplesProcessed, 0, FBFixedBlockSamples);
}

FBModuleGraphPlotParams
GlobalUniGraphProcessor::PlotParams(
  bool /*detailGraphs*/, int /*graphIndex*/) const
{
  FBModuleGraphPlotParams result = {};
  result.sampleRate = 0.0f;
  result.autoSampleRate = true;
  result.sampleCount = ComponentData()->pixelWidth * 2;
  result.staticModuleIndex = (int)FFModuleType::GlobalUni;
  return result;
}

void
GlobalUniGraphProcessor::BeginVoiceOrBlock(
  FBGraphRenderState* state,
  FBModuleGraphProcessParams const& params)
{
  samplesProcessed = 0;
  auto* moduleProcState = state->ModuleProcState();
  int voiceCount = state->AudioParamDiscrete(
    { { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } }, params.exchange, -1);
  GetProcessor(*moduleProcState).BeginBlock(*moduleProcState);
  for (int i = 0; i < voiceCount; i++)
    GetProcessor(*moduleProcState).BeginVoice(i);
}

void
GlobalUniGraphProcessor::ProcessIndicators(
  FBGraphRenderState* state,
  FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points)
{
  int slot = ComponentData()->fixedGraphIndex;
  FBSArray<float, FBFixedBlockSamples> targetSignal;
  FFProcDSPState* procState = &state->ModuleProcState()->ProcAs<FFProcState>()->dsp;
  FFGlobalUniProcessor* processor = procState->global.globalUni.processor.get();
  float targetDefault = FFGlobalUniTargetGetDefaultValue((FFGlobalUniTarget)slot);
  int voiceCount = state->AudioParamDiscrete({ { (int)FFModuleType::GlobalUni, 0 }, { (int)FFGlobalUniParam::VoiceCount, 0 } }, params.exchange, -1);
  for (int i = 0; i < voiceCount; i++)
  {
    targetSignal.Fill(FBBatch<float>(targetDefault));
    processor->ApplyToVoice(*state->ModuleProcState(), (FFGlobalUniTarget)slot, true, -1, i, targetSignal);
    points.pointIndicators.push_back((int)(targetSignal.Get(0) * points.l.size()));
  }
}

void 
GlobalUniGraphProcessor::PostProcess(
  FBGraphRenderState* /*state*/, FBModuleGraphData& /*data*/,
  FBModuleGraphProcessParams const& /*params*/, FBModuleGraphPoints& points)
{
  int slot = ComponentData()->fixedGraphIndex;
  if (FFGlobalUniTargetGetDefaultValue((FFGlobalUniTarget)slot) != 0.5f)
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
FFGlobalUniRenderGraph(FBModuleGraphComponentData* graphData, bool detailGraphs)
{
  GlobalUniGraphProcessor processor(graphData);
  graphData->pointIndicatorSize = 4;
  graphData->fillPointIndicators = true;
  graphData->skipDrawOnEqualsPrimary = true;
  graphData->drawMarkersSelector = [](int) { return false; };
  graphData->renderState->ModuleProcState()->moduleSlot = 0;
  graphData->paintAsDisabled = graphData->renderState->AudioParamList<FFModulationOpType>(
    { { (int)FFModuleType::GlobalUni, 0 },
    { (int)FFGlobalUniParam::OpType, graphData->fixedGraphIndex } }, false, -1) == FFModulationOpType::Off;
  graphData->paintAsDisabled |= graphData->renderState->AudioParamList<FFGlobalUniMode>(
    { { (int)FFModuleType::GlobalUni, 0 },
    { (int)FFGlobalUniParam::Mode, graphData->fixedGraphIndex } }, false, -1) == FFGlobalUniMode::Off;

  processor.totalSamples = processor.PlotParams(detailGraphs, 0).sampleCount;
  FBRenderModuleGraph(&processor, true, false, detailGraphs, 0);
}