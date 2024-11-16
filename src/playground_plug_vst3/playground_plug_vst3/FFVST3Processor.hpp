#pragma once
#include <playground_plug/plug/dsp/FFProcessor.hpp>
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3Processor :
public AudioEffect
{
  std::unique_ptr<FFProcessor> _processor = {};

public:  

  FFVST3Processor(FUID const& controllerId);

  tresult PLUGIN_API 
  process(ProcessData& data) override;
  tresult PLUGIN_API 
  initialize(FUnknown* context) override;
  tresult PLUGIN_API
  setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API 
  canProcessSampleSize(int32 symbolicSize) override;
  tresult PLUGIN_API 
  setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
};