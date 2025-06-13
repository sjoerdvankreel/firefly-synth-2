#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>
#include <firefly_base_vst3/FBVST3EditController.hpp>

#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContainer.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#if SMTG_OS_LINUX
#include <juce_events/native/juce_EventLoopInternal_linux.h>
#endif

#include <algorithm>

using namespace juce;

FBVST3GUIEditor::
FBVST3GUIEditor(FBVST3EditController* editController):
EditorView(editController),
_hostContext(editController),
_gui(std::make_unique<FBPlugGUIContainer>(editController))
{
  FB_LOG_ENTRY_EXIT();
}

FBVST3GUIEditor::
~FBVST3GUIEditor()
{
  FB_LOG_ENTRY_EXIT();
  dynamic_cast<FBVST3EditController&>(*getController()).ResetView();
}

void 
FBVST3GUIEditor::UpdateExchangeState()
{
  _gui->UpdateExchangeState();
}

void
FBVST3GUIEditor::SetAudioParamNormalizedFromHost(int index, double normalized)
{
  _gui->SetAudioParamNormalizedFromHost(index, normalized);
}

tresult PLUGIN_API
FBVST3GUIEditor::canResize()
{
  return kResultTrue;
}

uint32 PLUGIN_API 
FBVST3GUIEditor::addRef() 
{ 
  return EditorView::addRef(); 
}

uint32 PLUGIN_API 
FBVST3GUIEditor::release()
{ 
  return EditorView::release(); 
}

tresult PLUGIN_API
FBVST3GUIEditor::getSize(ViewRect* size)
{
  return FBWithLogException([this, size]()
  {
    auto hostSize = _gui->GetHostSize();
    size->right = size->left + hostSize.first;
    size->bottom = size->top + hostSize.second;
    return kResultTrue;
  });
}

tresult PLUGIN_API
FBVST3GUIEditor::onSize(ViewRect* newSize)
{
  return FBWithLogException([this, newSize]()
  {
    checkSizeConstraint(newSize);
    _gui->SetUserScaleByHostWidth(newSize->getWidth());
    return kResultTrue;
  });
}

#if SMTG_OS_LINUX
void PLUGIN_API
FBVST3GUIEditor::onFDIsSet(FileDescriptor fd)
{
  FBWithLogException([fd]() { LinuxEventLoopInternal::invokeEventLoopCallbackForFd(fd); });
}
#endif

tresult PLUGIN_API
FBVST3GUIEditor::setContentScaleFactor(ScaleFactor factor)
{
#if __APPLE__
  return kResultFalse;
#endif
  return FBWithLogException([this, factor]()
  {
    ViewRect newSize;
    _gui->SetSystemScale(factor);
    getSize(&newSize);
    if (plugFrame)
      plugFrame->resizeView(this, &newSize);
    return kResultTrue;
  });
}

tresult PLUGIN_API
FBVST3GUIEditor::removed()
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this]()
  {
    _gui->RemoveFromDesktop();
#if SMTG_OS_LINUX
    IRunLoop* loop = {};
    auto ok = plugFrame->queryInterface(IRunLoop::iid, (void**)&loop);
    FB_ASSERT(ok == kResultOk);
    loop->unregisterEventHandler(this);
#endif
    return EditorView::removed();
  });
}

tresult PLUGIN_API
FBVST3GUIEditor::attached(void* parent, FIDString type)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, parent, type]()
  {
#if SMTG_OS_LINUX
    IRunLoop* loop = {};
    auto ok = plugFrame->queryInterface(IRunLoop::iid, (void**)&loop);
    FB_ASSERT(ok == kResultOk);
    for (int fd : LinuxEventLoopInternal::getRegisteredFds())
      loop->registerEventHandler(this, fd);
#endif
    _gui->AddToDesktop(parent);
    return EditorView::attached(parent, type);
  });
}

tresult PLUGIN_API 
FBVST3GUIEditor::checkSizeConstraint(ViewRect* rect_)
{
  return FBWithLogException([this, rect_]()
  {
    rect_->right = rect_->left + _gui->ClampHostWidthForScale(rect_->getWidth());
    rect_->bottom = rect_->top + _gui->GetHeightForAspectRatio(rect_->getWidth());
    return kResultTrue;
  });
}

tresult PLUGIN_API 
FBVST3GUIEditor::isPlatformTypeSupported(FIDString type)
{
  if (std::string(type) == kPlatformTypeHWND) return kResultTrue;
  if (std::string(type) == kPlatformTypeNSView) return kResultTrue;
  if (std::string(type) == kPlatformTypeX11EmbedWindowID) return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
FBVST3GUIEditor::queryInterface(TUID const iid, void** obj)
{
#if SMTG_OS_LINUX
  QUERY_INTERFACE(iid, obj, IEventHandler::iid, IEventHandler);
#endif
  QUERY_INTERFACE(iid, obj, IPlugViewContentScaleSupport::iid, IPlugViewContentScaleSupport);
  return EditorView::queryInterface(iid, obj);
}

IPtr<IContextMenu>
FBVST3GUIEditor::MakeVSTMenu(
  IPtr<IComponentHandler> handler, int paramIndex)
{
  return FBWithLogException([this, handler, paramIndex]()
  {
    ParamID paramTag = _hostContext->Topo()->audio.params[paramIndex].tag;
    FUnknownPtr<IComponentHandler3> handler3(handler);
    if (handler3 == nullptr)
      return IPtr<IContextMenu>();
    return IPtr<IContextMenu>(handler3->createContextMenu(this, &paramTag));
  });
}

void 
FBVST3GUIEditor::ParamContextMenuClicked(
  IPtr<IComponentHandler> handler, int paramIndex, int juceTag)
{
  FBWithLogException([this, handler, paramIndex, juceTag]()
  {
    IContextMenu::Item item = {};
    IContextMenuTarget* target = nullptr;
    auto vstMenu = MakeVSTMenu(handler, paramIndex);
    if (vstMenu && vstMenu->getItem(juceTag - 1, item, &target) == kResultOk && target != nullptr)
      target->executeMenuItem(item.tag);
  });
}

std::vector<FBHostContextMenuItem>
FBVST3GUIEditor::MakeParamContextMenu(
  IPtr<IComponentHandler> handler, int index)
{
  return FBWithLogException([this, handler, index]
  {
    auto vstMenu = MakeVSTMenu(handler, index);
    if (!vstMenu)
      return std::vector<FBHostContextMenuItem>();

    IContextMenu::Item vstItem = {};
    FBHostContextMenuItem item = {};
    IContextMenuTarget* target = nullptr;
    std::vector<FBHostContextMenuItem> result = {};
    for (int i = 0; i < vstMenu->getItemCount(); i++)
    {
      if (vstMenu->getItem(i, vstItem, &target) != kResultOk)
      {
        FB_ASSERT(false);
        return std::vector<FBHostContextMenuItem>();
      }
      FBVST3CopyFromString128(vstItem.name, item.name);
      item.checked = (vstItem.flags & IContextMenuItem::kIsChecked) == IContextMenuItem::kIsChecked;
      item.enabled = (vstItem.flags & IContextMenuItem::kIsDisabled) != IContextMenuItem::kIsDisabled;
      item.separator = (vstItem.flags & IContextMenuItem::kIsSeparator) == IContextMenuItem::kIsSeparator;
      item.subMenuEnd = (vstItem.flags & IContextMenuItem::kIsGroupEnd) == IContextMenuItem::kIsGroupEnd;
      item.subMenuStart = (vstItem.flags & IContextMenuItem::kIsGroupStart) == IContextMenuItem::kIsGroupStart;
      result.push_back(item);
    }
    return result;
  });
}