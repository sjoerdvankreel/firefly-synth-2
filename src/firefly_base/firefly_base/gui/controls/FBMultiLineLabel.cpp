#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/controls/FBMultiLineLabel.hpp>

using namespace juce;

FBAutoSizeMultiLineLabel::
FBAutoSizeMultiLineLabel(FBPlugGUI* plugGUI, std::string const& text, int vOffset):
Component(),
IFBHorizontalAutoSize(),
_plugGUI(plugGUI),
_vOffset(vOffset),
_text(text) {}

int 
FBAutoSizeMultiLineLabel::FixedWidth(int /*height*/) const
{
  return FBGetLookAndFeelFor(_plugGUI)->GetStringWidthCached(_text) + 6;
}

void
FBAutoSizeMultiLineLabel::paint(Graphics& g)
{
  Label dummy;
  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  auto textSize = lnf->GetStringSizeCached(_text);
  auto const& scheme = lnf->FindColorSchemeFor(*this);
  g.setFont(getLookAndFeel().getLabelFont(dummy));
  g.setColour(scheme.text);
  int y = static_cast<int>(std::ceil((getBounds().getHeight() - textSize.y) * 0.5f) + _vOffset);
  g.drawMultiLineText(_text, 3, y, textSize.x + 4, Justification::centred);
}