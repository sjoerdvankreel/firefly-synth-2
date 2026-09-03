#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBParamNameEditor.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

// 126 is quite arbitrary,
// but it's the max bitwig accepts before reverting to generic names
FBParamNameEditor::
FBParamNameEditor(FBPlugGUI* plugGUI, int):
_plugGUI(plugGUI), _filter(126, "")
{
  auto lnf = FBGetLookAndFeelFor(plugGUI);
  addListener(this);
  setFont(lnf->GetFont());
  setPopupMenuEnabled(false);
  setInputFilter(&_filter, false);
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
FBParamNameEditor::ClearEdit(int paramIndex)
{
  setText("");
  _paramIndex = paramIndex;
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