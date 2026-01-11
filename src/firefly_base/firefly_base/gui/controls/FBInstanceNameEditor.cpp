#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBInstanceNameEditor.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FBInstanceNameEditor::
FBInstanceNameEditor(FBPlugGUI* plugGUI, int fixedWidth):
_fixedWidth(fixedWidth), _plugGUI(plugGUI)
{
  setText(_plugGUI->HostContext()->InstanceName(), dontSendNotification);
  addListener(this);
  setFont(FBGUIGetFont());
  setIndents(2, 8);
  setPopupMenuEnabled(false);
}

int
FBInstanceNameEditor::FixedHeight() const
{
  return 24;
}

int
FBInstanceNameEditor::FixedWidth(int /*height*/) const
{
  return _fixedWidth;
}

void 
FBInstanceNameEditor::paint(Graphics& g)
{
  auto const& scheme = FBGetLookAndFeelFor(this)->FindColorSchemeFor(*this);
  setColour(TextEditor::textColourId, scheme.primary);
  applyFontToAllText(getFont());
  TextEditor::paint(g);
}

void 
FBInstanceNameEditor::textEditorTextChanged(TextEditor&)
{
  _plugGUI->HostContext()->SetInstanceName(getText().toStdString());
}