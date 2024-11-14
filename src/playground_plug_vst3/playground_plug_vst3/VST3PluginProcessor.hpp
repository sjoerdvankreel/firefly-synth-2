#pragma once
#include <public.sdk/source/vst/vstaudioeffect.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class VST3PluginProcessor :
public AudioEffect
{
public:
  tresult PLUGIN_API process(ProcessData& data) override;
};