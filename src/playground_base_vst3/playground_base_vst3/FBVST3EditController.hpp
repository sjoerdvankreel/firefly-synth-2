#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBStringify.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBStaticTopo;
struct FBRuntimeTopo;

class FBVST3EditController final:
public EditControllerEx1,
public IFBHostGUIContext
{
  std::unique_ptr<FBRuntimeTopo> _topo;

public:
  FBVST3EditController(FBStaticTopo const& topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3EditController);

  tresult PLUGIN_API setState(IBStream* state) override;
  tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  IPlugView* PLUGIN_API createView(FIDString name) override;
  
  void EndParamChange(int index) override;
  void BeginParamChange(int index) override;
  float GetParamNormalized(int index) const override;
  void SetParamNormalized(int index, float normalized) override;
};