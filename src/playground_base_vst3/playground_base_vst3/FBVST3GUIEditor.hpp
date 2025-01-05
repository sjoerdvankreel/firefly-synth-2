#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <public.sdk/source/vst/vsteditcontroller.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FBVST3EditController;

class FBVST3GUIEditor final:
public EditorView
{
  FBPlugGUIFactory const _factory;
  IFBHostGUIContext* const _hostContext;

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBVST3GUIEditor);
  FBVST3GUIEditor(
    FBPlugGUIFactory const& factory, 
    FBVST3EditController* editController);

  tresult PLUGIN_API removed() override;
  tresult PLUGIN_API canResize() override;
  tresult PLUGIN_API onSize(ViewRect* new_size) override;
  tresult PLUGIN_API getSize(ViewRect* new_size) override;
  tresult PLUGIN_API checkSizeConstraint(ViewRect* rect) override;
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
};