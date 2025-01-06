#include <playground_base_vst3/FBVST3GUIEditor.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>

#include <algorithm>

FBVST3GUIEditor::
FBVST3GUIEditor(
  FBPlugGUIFactory const& factory,
  FBRuntimeTopo const* topo,
  FBVST3EditController* editController):
EditorView(editController),
_gui(factory(topo, editController)) {}

tresult PLUGIN_API 
FBVST3GUIEditor::canResize()
{
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3GUIEditor::removed()
{
  _gui->removeFromDesktop();
  return EditorView::removed();
}

tresult PLUGIN_API
FBVST3GUIEditor::attached(void* parent, FIDString type)
{
  _gui->addToDesktop(0, parent);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API 
FBVST3GUIEditor::getSize(ViewRect* size)
{
  size->right = size->left + _gui->getWidth();
  size->bottom = size->top + _gui->getHeight();
  checkSizeConstraint(size);
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3GUIEditor::onSize(ViewRect* newSize)
{
  checkSizeConstraint(newSize);
  _gui->setSize(newSize->getWidth(), newSize->getHeight());
  return kResultTrue;
}

void
FBVST3GUIEditor::SetParamNormalized(int index, float normalized)
{
  _gui->SetParamNormalized(index, normalized);
}

tresult PLUGIN_API 
FBVST3GUIEditor::checkSizeConstraint(ViewRect* rect)
{
  int minW = _gui->MinWidth();
  int maxW = _gui->MaxWidth();
  int arW = _gui->AspectRatioWidth();
  int arH = _gui->AspectRatioHeight();
  rect->right = rect->left + std::clamp(rect->getWidth(), minW, maxW);
  rect->bottom = rect->top + rect->getWidth() * arH / arW;
  return kResultTrue;
}

tresult PLUGIN_API 
FBVST3GUIEditor::isPlatformTypeSupported(FIDString type)
{
  if (std::string(type) == kPlatformTypeHWND) return true;
  if (std::string(type) == kPlatformTypeNSView) return true;
  if (std::string(type) == kPlatformTypeX11EmbedWindowID) return true;
  return false;
}