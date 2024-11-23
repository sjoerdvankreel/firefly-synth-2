#pragma once

#include <playground_plug/base/shared/FFBaseTopo.hpp>
#include <playground_plug/base/shared/FFHostBlock.hpp>
#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>

#include <public.sdk/source/vst/vstaudioeffect.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3PluginProcessor :
public AudioEffect
{
  FFRuntimeTopo const _topo;
  FFRawStereoBlockView _output = {};
  std::unique_ptr<FFHostInputBlock> _input = {};
  std::unique_ptr<FFDynamicStereoBlock> _zeroIn = {};
  std::unique_ptr<FFPluginProcessor> _processor = {};

public:  
  FFVST3PluginProcessor(FUID const& controllerId);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFVST3PluginProcessor);

  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSize) override;

  tresult PLUGIN_API setBusArrangements(
    SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
};