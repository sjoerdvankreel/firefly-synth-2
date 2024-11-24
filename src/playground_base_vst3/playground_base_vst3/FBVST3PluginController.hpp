#pragma once

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <playground_base/shared/FBSharedUtility.hpp>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FBVST3PluginController:
public EditControllerEx1
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVST3PluginController);
  tresult PLUGIN_API initialize(FUnknown* context) override;
};