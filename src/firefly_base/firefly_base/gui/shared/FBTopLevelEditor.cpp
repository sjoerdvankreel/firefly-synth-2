#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBTopLevelEditor.hpp>

FBTopLevelEditor::
FBTopLevelEditor(FBPlugGUI* plugGUI, int id, LaunchOptions& options):
DialogWindow(
  options.dialogTitle, options.dialogBackgroundColour,
  options.escapeKeyTriggersCloseButton, true, 1.0f),
_id(id),
_plugGUI(plugGUI)
{
  setAlwaysOnTop(false);
  setUsingNativeTitleBar(options.useNativeTitleBar);
  setContentNonOwned(options.content.release(), true);
  setResizable(options.resizable, options.useBottomRightCornerResizer);
}

void 
FBTopLevelEditor::closeButtonPressed()
{
  _plugGUI->CloseTopLevelEditor(_id);
}