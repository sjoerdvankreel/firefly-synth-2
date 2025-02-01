#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/glue/FBHostContextMenu.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <pluginterfaces/vst/ivstcontextmenu.h>
#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>

#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;
#if SMTG_OS_LINUX
using namespace Steinberg::Linux;
#endif

class FBPlugGUIContext;
class FBHostGUIContext;
class FBVST3EditController;

class FBVST3GUIEditor final:
public EditorView,
public IPlugViewContentScaleSupport
#if SMTG_OS_LINUX
, public IEventHandler
#endif
{
  FBHostGUIContext* const _hostContext;
  std::unique_ptr<FBPlugGUIContext> _gui;

  IPtr<IContextMenu> 
  MakeVSTMenu(IPtr<IComponentHandler> handler, int paramIndex);

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBVST3GUIEditor);
  ~FBVST3GUIEditor();
  FBVST3GUIEditor(FBVST3EditController* editController);

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

  void UpdateExchangeState();
  void SetParamNormalized(int index, float normalized);
  void ParamContextMenuClicked(IPtr<IComponentHandler> handler, int paramIndex, int juceTag);
  std::vector<FBHostContextMenuItem> MakeParamContextMenu(IPtr<IComponentHandler> handler, int index);
};