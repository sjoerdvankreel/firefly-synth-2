#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>

#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;
#if SMTG_OS_LINUX
using namespace Steinberg::Linux;
#endif

struct FBGUIState;
struct FBRuntimeTopo;
struct FBStaticTopoGUI;

class FBPlugGUIContext;
class FBVST3EditController;

class FBVST3GUIEditor final:
public EditorView,
public IPlugViewContentScaleSupport
#if SMTG_OS_LINUX
, public IEventHandler
#endif
{
  std::unique_ptr<FBPlugGUIContext> _gui;

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBVST3GUIEditor);
  ~FBVST3GUIEditor();
  FBVST3GUIEditor(
    FBPlugGUIFactory const& factory, 
    FBRuntimeTopo const* topo,
    FBGUIState* guiState,
    FBVST3EditController* editController);

  void SetParamNormalized(int index, float normalized);

#if SMTG_OS_LINUX
  void PLUGIN_API onFDIsSet(FileDescriptor fd) override;
#endif

  tresult PLUGIN_API removed() override;
  tresult PLUGIN_API canResize() override;
  tresult PLUGIN_API getSize(ViewRect* size) override;
  tresult PLUGIN_API onSize(ViewRect* newSize) override;
  tresult PLUGIN_API checkSizeConstraint(ViewRect* rect) override;
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
  
  uint32 PLUGIN_API addRef() override;
  uint32 PLUGIN_API release() override;
  tresult PLUGIN_API setContentScaleFactor(ScaleFactor factor) override;
  tresult PLUGIN_API queryInterface(Steinberg::TUID const iid, void** obj) override;
};