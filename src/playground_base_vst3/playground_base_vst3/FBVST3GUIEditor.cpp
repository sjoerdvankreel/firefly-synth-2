#include <playground_base_vst3/FBVST3GUIEditor.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <algorithm>

FBVST3GUIEditor::
FBVST3GUIEditor(
  FBPlugGUIFactory const& factory,
  FBRuntimeTopo const* topo,
  FBVST3EditController* editController):
EditorView(editController),
_gui(factory(topo, editController)) {}

FBVST3GUIEditor::
~FBVST3GUIEditor()
{
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
FBVST3GUIEditor::removed()
{
  _gui->RemoveFromDesktop();
  return EditorView::removed();
}

tresult PLUGIN_API
FBVST3GUIEditor::attached(void* parent, FIDString type)
{
  _gui->AddToDesktop(parent);
  return EditorView::attached(parent, type);
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
FBVST3GUIEditor::checkSizeConstraint(ViewRect* rect)
{
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
  QUERY_INTERFACE(iid, obj, IPlugViewContentScaleSupport::iid, IPlugViewContentScaleSupport);
  return EditorView::queryInterface(iid, obj);
}