#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/dsp/FBHostBlockProcessor.hpp>

#include <public.sdk/source/vst/vstaudioeffect.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FBVST3AudioEffect :
public AudioEffect
{
  FBRuntimeTopo const _topo;
  std::unique_ptr<FBHostInputBlock> _input = {};
  std::unique_ptr<FBDynamicAudioBlock> _zeroIn = {};
  std::unique_ptr<IFBHostBlockProcessor> _processor = {};

protected:
  virtual std::unique_ptr<IFBHostBlockProcessor> CreateProcessor(
    FBRuntimeTopo const& topo, ProcessSetup const& setup) const = 0;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3AudioEffect);
  FBVST3AudioEffect(FBRuntimeTopo&& topo, FUID const& controllerId);

  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSize) override;

  tresult PLUGIN_API setBusArrangements(
    SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
};