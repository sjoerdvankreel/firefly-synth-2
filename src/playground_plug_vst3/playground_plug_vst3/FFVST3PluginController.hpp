#pragma once

#include <playground_plug/base/shared/FFUtility.hpp>
#include <public.sdk/source/vst/vsteditcontroller.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3PluginController:
public EditControllerEx1
{
public:
  FF_NOCOPY_NOMOVE_DEFCTOR(FFVST3PluginController);
  tresult PLUGIN_API initialize(FUnknown* context) override;
};