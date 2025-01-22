#include <playground_base_vst3/FBVST3Utility.hpp>
#include <playground_base_vst3/FBVST3GUIEditor.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>

#include <playground_base/base/shared/FBLogging.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/glue/FBPlugGUIContainer.hpp>

#include <pluginterfaces/vst/ivstcontextmenu.h>

#if SMTG_OS_LINUX
#include <juce_events/native/juce_EventLoopInternal_linux.h>
#endif

#include <stack>
#include <algorithm>

using namespace juce;

struct MenuBuilder
{
  bool checked;
  bool enabled;
  std::string name;
  std::unique_ptr<PopupMenu> menu;
};

FBVST3GUIEditor::
FBVST3GUIEditor(
  FBPlugGUIFactory const& factory,
  FBRuntimeTopo const* topo,
  FBVST3EditController* editController,
  FBGUIState* guiState):
EditorView(editController),
_topo(topo),
_gui(std::make_unique<FBPlugGUIContainer>(topo, editController, guiState))
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
FBVST3GUIEditor::SetParamNormalized(int index, float normalized)
{
  _gui->SetParamNormalized(index, normalized);
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

std::unique_ptr<PopupMenu>
FBVST3GUIEditor::ParamContextMenu(IPtr<IComponentHandler> handler, int index)
{
  FUnknownPtr<IComponentHandler3> handler3(handler);
  if (handler3 == nullptr) 
    return {};
  
  ParamID tag = _topo->params[index].tag;
  IPtr<IContextMenu> vstMenu(handler3->createContextMenu(this, &tag));
  if (!vstMenu) 
    return {};

  IContextMenu::Item vstItem = {};
  IContextMenuTarget* target = nullptr;
  std::stack<MenuBuilder> builders = {};
  builders.emplace();
  builders.top().menu = std::make_unique<PopupMenu>();
  for (int i = 0; i < vstMenu->getItemCount(); i++)
  {
    if (vstMenu->getItem(i, vstItem, &target) != kResultOk) 
      return {};

    std::string name;
    if (!FBVST3CopyFromString128(vstItem.name, name))
      return {};
    bool checked = (vstItem.flags & IContextMenuItem::kIsChecked) == IContextMenuItem::kIsChecked;
    bool disabled = (vstItem.flags & IContextMenuItem::kIsDisabled) == IContextMenuItem::kIsDisabled;
    bool groupEnd = (vstItem.flags & IContextMenuItem::kIsGroupEnd) == IContextMenuItem::kIsGroupEnd;
    bool separator = (vstItem.flags & IContextMenuItem::kIsSeparator) == IContextMenuItem::kIsSeparator;
    bool groupStart = (vstItem.flags & IContextMenuItem::kIsGroupStart) == IContextMenuItem::kIsGroupStart;
   
    if (groupStart)
    {
      builders.emplace();
      builders.top().name = name;
      builders.top().checked = checked;
      builders.top().enabled = !disabled;
      builders.top().menu = std::make_unique<PopupMenu>();
    }
    else if (groupEnd)
    {
      auto builder = std::move(builders.top());
      builders.pop();
      builders.top().menu->addSubMenu(builder.name, *builder.menu, builder.enabled, nullptr, builder.checked);
    } else if(separator)
      builders.top().menu->addSeparator();
    else
      builders.top().menu->addItem(1, name, !disabled, checked);
  }
  assert(builders.size() == 1);
  return std::move(builders.top().menu);
}