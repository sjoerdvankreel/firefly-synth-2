#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FBPlugGUI;
class FBVST3EditController;
struct FBRuntimeTopo;

class FBVST3GUIEditor final:
public EditorView
{
  std::unique_ptr<FBPlugGUI> _gui;

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBVST3GUIEditor);
  FBVST3GUIEditor(
    FBPlugGUIFactory const& factory, 
    FBRuntimeTopo const* topo,
    FBVST3EditController* editController);

  void SetParamNormalized(int index, float normalized);

  tresult PLUGIN_API removed() override;
  tresult PLUGIN_API canResize() override;
  tresult PLUGIN_API getSize(ViewRect* size) override;
  tresult PLUGIN_API onSize(ViewRect* newSize) override;
  tresult PLUGIN_API checkSizeConstraint(ViewRect* rect) override;
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
};