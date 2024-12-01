#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBStringify.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FBRuntimeTopo;

class FBVST3EditController :
public EditControllerEx1
{
  std::unique_ptr<FBRuntimeTopo> _topo;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3EditController);
  FBVST3EditController(std::unique_ptr<FBRuntimeTopo>&& topo);
  tresult PLUGIN_API initialize(FUnknown* context) override;
};