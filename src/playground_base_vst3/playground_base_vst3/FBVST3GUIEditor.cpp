#include <playground_base_vst3/FBVST3GUIEditor.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>

FBVST3GUIEditor::
FBVST3GUIEditor(
  FBPlugGUIFactory const& factory,
  FBVST3EditController* editController):
EditorView(editController),
_gui(factory(editController)) {}

tresult PLUGIN_API 
FBVST3GUIEditor::removed()
{
}

tresult PLUGIN_API 
FBVST3GUIEditor::canResize()
{
  return kResultTrue;
}

tresult PLUGIN_API 
FBVST3GUIEditor::onSize(ViewRect* new_size)
{

}

tresult PLUGIN_API 
FBVST3GUIEditor::getSize(ViewRect* new_size)
{
}

tresult PLUGIN_API 
FBVST3GUIEditor::checkSizeConstraint(ViewRect* rect)
{
}

tresult PLUGIN_API 
FBVST3GUIEditor::attached(void* parent, FIDString type)
{
}

tresult PLUGIN_API 
FBVST3GUIEditor::isPlatformTypeSupported(FIDString type)
{
}