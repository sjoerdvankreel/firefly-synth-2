#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBStringify.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBStaticTopo;
struct FBRuntimeTopo;

class FBVST3EditController final:
public EditControllerEx1
{
  std::unique_ptr<FBRuntimeTopo> _topo;

public:
  FBVST3EditController(FBStaticTopo const& topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3EditController);
  tresult PLUGIN_API initialize(FUnknown* context) override;
};