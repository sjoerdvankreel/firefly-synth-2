#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBParamNameEditor.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FBParamNameEditor::
FBParamNameEditor(FBPlugGUI* plugGUI, int):
_plugGUI(plugGUI)
{
  auto lnf = FBGetLookAndFeelFor(plugGUI);
  addListener(this);
  setFont(lnf->GetFont());
  setPopupMenuEnabled(false);
}

void 
FBParamNameEditor::paint(Graphics& g)
{
  auto const& scheme = FBGetLookAndFeelFor(_plugGUI)->FindColorSchemeFor(*this);
  setColour(TextEditor::textColourId, scheme.primary);
  applyFontToAllText(getFont());
  TextEditor::paint(g);
}

void 
FBParamNameEditor::textEditorTextChanged(TextEditor&)
{
  if (_paramIndex != -1)
  {
    if(getText().isEmpty())
      _plugGUI->HostContext()->ClearAudioParamNameOverride(_paramIndex);
    else
      _plugGUI->HostContext()->SetAudioParamNameOverride(_paramIndex, getText().toStdString());
    _plugGUI->HostContext()->NotifyHostOfParamNameChanges();
  }
}

void 
FBParamNameEditor::InitEdit(int paramIndex)
{
  setText("");
  std::string name;
  _paramIndex = paramIndex;
  if (_plugGUI->HostContext()->GetAudioParamNameOverride(paramIndex, name))
    setText(name);
}