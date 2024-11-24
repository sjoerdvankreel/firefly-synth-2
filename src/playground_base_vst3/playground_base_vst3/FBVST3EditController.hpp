#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FBVST3EditController :
public EditControllerEx1
{
  FBRuntimeTopo const _topo;

public:
  FBVST3EditController(FBRuntimeTopo&& topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3EditController);
  tresult PLUGIN_API initialize(FUnknown* context) override;
};