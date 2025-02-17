#include <playground_base_vst3/FBVST3Utility.hpp>
#include <playground_base_vst3/FBVST3GUIEditor.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>

#include <playground_base/gui/glue/FBHostContextMenu.hpp>
#include <playground_base/gui/glue/FBPlugGUIContainer.hpp>
#include <playground_base/base/shared/FBLogging.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

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
FBVST3GUIEditor::SetAudioParamNormalizedFromHost(int index, float normalized)
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
  auto hostSize = _gui->GetHostSize();
  size->right = size->left + hostSize.first;
  size->bottom = size->top + hostSize.second;
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3GUIEditor::onSize(ViewRect* newSize)
{
  checkSizeConstraint(newSize);
  _gui->SetUserScaleByHostWidth(newSize->getWidth());
  return kResultTrue;
}

#if SMTG_OS_LINUX
void PLUGIN_API
FBVST3GUIEditor::onFDIsSet(FileDescriptor fd)
{
  LinuxEventLoopInternal::invokeEventLoopCallbackForFd(fd);
}
#endif

tresult PLUGIN_API
FBVST3GUIEditor::setContentScaleFactor(ScaleFactor factor)
{
#if __APPLE__
  return kResultFalse;
#endif
  ViewRect newSize;
  _gui->SetSystemScale(factor);
  getSize(&newSize);
  if (plugFrame)
    plugFrame->resizeView(this, &newSize);
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3GUIEditor::removed()
{
  FB_LOG_ENTRY_EXIT();
  _gui->RemoveFromDesktop();
#if SMTG_OS_LINUX
  IRunLoop* loop = {};
  auto ok = plugFrame->queryInterface(IRunLoop::iid, (void**)&loop);
  assert(ok == kResultOk);
  loop->unregisterEventHandler(this);
#endif
  return EditorView::removed();
}

tresult PLUGIN_API
FBVST3GUIEditor::attached(void* parent, FIDString type)
{
  FB_LOG_ENTRY_EXIT();
#if SMTG_OS_LINUX
  IRunLoop* loop = {};
  auto ok = plugFrame->queryInterface(IRunLoop::iid, (void**)&loop);
  assert(ok == kResultOk);
  for (int fd : LinuxEventLoopInternal::getRegisteredFds())
    loop->registerEventHandler(this, fd);
#endif
  _gui->AddToDesktop(parent);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API 
FBVST3GUIEditor::checkSizeConstraint(ViewRect* rect)
{
  rect->right = rect->left + _gui->ClampHostWidthForScale(rect->getWidth());
  rect->bottom = rect->top + _gui->GetHeightForAspectRatio(rect->getWidth());
  return kResultTrue;
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
  ParamID paramTag = _hostContext->Topo()->audio.params[paramIndex].tag;
  FUnknownPtr<IComponentHandler3> handler3(handler);
  if (handler3 == nullptr)
    return {};
  return handler3->createContextMenu(this, &paramTag);
}

void 
FBVST3GUIEditor::ParamContextMenuClicked(
  IPtr<IComponentHandler> handler, int paramIndex, int juceTag)
{
  IContextMenu::Item item = {};
  IContextMenuTarget* target = nullptr;
  auto vstMenu = MakeVSTMenu(handler, paramIndex);
  if (vstMenu && vstMenu->getItem(juceTag - 1, item, &target) == kResultOk && target != nullptr)
    target->executeMenuItem(item.tag);
}

std::vector<FBHostContextMenuItem>
FBVST3GUIEditor::MakeParamContextMenu(
  IPtr<IComponentHandler> handler, int index)
{
  auto vstMenu = MakeVSTMenu(handler, index);
  if (!vstMenu) 
    return {};

  IContextMenu::Item vstItem = {};
  FBHostContextMenuItem item = {};
  IContextMenuTarget* target = nullptr;
  std::vector<FBHostContextMenuItem> result = {};
  for (int i = 0; i < vstMenu->getItemCount(); i++)
  {
    if (vstMenu->getItem(i, vstItem, &target) != kResultOk) 
      return {};
    if (!FBVST3CopyFromString128(vstItem.name, item.name))
      return {};
    item.checked = (vstItem.flags & IContextMenuItem::kIsChecked) == IContextMenuItem::kIsChecked;
    item.enabled = (vstItem.flags & IContextMenuItem::kIsDisabled) != IContextMenuItem::kIsDisabled;
    item.separator = (vstItem.flags & IContextMenuItem::kIsSeparator) == IContextMenuItem::kIsSeparator;
    item.subMenuEnd = (vstItem.flags & IContextMenuItem::kIsGroupEnd) == IContextMenuItem::kIsGroupEnd;
    item.subMenuStart = (vstItem.flags & IContextMenuItem::kIsGroupStart) == IContextMenuItem::kIsGroupStart;
    result.push_back(item);
  }
  return result;
}