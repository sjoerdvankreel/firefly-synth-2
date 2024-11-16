#pragma once
#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <public.sdk/source/vst/vstaudioeffect.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3PluginProcessor :
public AudioEffect
{
  FFPluginProcessor _processor;

public:  

  FFVST3PluginProcessor(FUID const& controllerId);

  tresult PLUGIN_API 
  process(ProcessData& data) override;
  tresult PLUGIN_API 
  initialize(FUnknown* context) override;
  tresult PLUGIN_API 
  canProcessSampleSize(int32 symbolicSize) override;
  tresult PLUGIN_API 
  setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
};