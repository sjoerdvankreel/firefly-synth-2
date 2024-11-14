#pragma once
#include <playground_plug/FFPluginProcessor.hpp>
#include <public.sdk/source/vst/vstaudioeffect.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3PluginProcessor :
public AudioEffect
{
  FFPluginProcessor _processor;
public:  
  tresult PLUGIN_API 
  process(ProcessData& data) override;
};