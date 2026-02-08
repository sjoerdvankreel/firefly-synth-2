#pragma once

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>

#include <firefly_base/base/state/main/FBGraphRenderState.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>

#include <vector>

class FBGraphRenderState;
class FBModuleGraphProcessor;

struct FBModuleGraphComponentData;
struct FBModuleProcExchangeStateBase;

struct FBModuleGraphStateParams
{
  int voice = {};
  int moduleSlot = {};
  int graphIndex = {};
  bool global = {};
  bool detailGraphs = {};
};

struct FBModuleGraphPlotParams
{
  int sampleCount = {};
  float sampleRate = {};
  bool autoSampleRate = {};
  int staticModuleIndex = -1;
};

struct FBModuleGraphProcessParams
{
  bool exchange = {};
  bool detailGraphs = {};
  int graphIndex = {};
  int exchangeVoice = {};
  float guiSampleRate = {};
  float hostSampleRate = {};
};

void
FBRenderModuleGraph(
  FBModuleGraphProcessor* processor,
  bool global, bool stereo,
  bool detailGraphs, int graphIndex);

class FBModuleGraphProcessor
{
  FBModuleGraphComponentData* const _componentData;

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBModuleGraphProcessor);
  FBModuleGraphProcessor(FBModuleGraphComponentData* componentData) :
  _componentData(componentData) {}

public:
  FBModuleGraphComponentData* ComponentData() { return _componentData; }

  virtual FBModuleGraphPlotParams PlotParams(
    bool detailGraphs, int graphIndex) const = 0;
  virtual FBModuleProcExchangeStateBase const* ExchangeState(
    void const* /*exchangeState*/, FBModuleGraphStateParams const& /*params*/) { return nullptr; }
  virtual FBSArray<float, FBFixedBlockSamples> const* MonoOutput(
    void const* /*procState*/, FBModuleGraphStateParams const& /*params*/) { return nullptr; }
  virtual FBSArray2<float, FBFixedBlockSamples, 2> const* StereoOutput(
    void const* /*procState*/, FBModuleGraphStateParams const& /*params*/) { return nullptr; }

  virtual int Process(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) = 0;
  virtual void BeginVoiceOrBlock(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params) = 0;
  virtual void ReleaseOnDemandBuffers(FBGraphRenderState* state, 
    FBModuleGraphProcessParams const& params) = 0;
  virtual void ProcessIndicators(FBGraphRenderState* state,
    FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) = 0;
  virtual void PostProcess(FBGraphRenderState* state, 
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleGraphPoints& points) = 0;
  virtual void ProcessExchangeState(FBGraphRenderState* graphState,
    FBModuleGraphData& data, FBModuleGraphProcessParams const& params, FBModuleProcExchangeStateBase const* exchangeState) = 0;
};