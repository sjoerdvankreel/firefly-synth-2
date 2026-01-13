#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/controls/FBMultiLineLabel.hpp>

using namespace juce;

FBAutoSizeMultiLineLabel::
FBAutoSizeMultiLineLabel(std::string const& text, int vOffset):
Component(),
IFBHorizontalAutoSize(),
_vOffset(vOffset),
_text(text) {}

int 
FBAutoSizeMultiLineLabel::FixedWidth(int /*height*/) const
{
  return FBGetLookAndFeelFor(this)->GetStringWidthCached(_text) + 4;
}

void
FBAutoSizeMultiLineLabel::paint(Graphics& g)
{
  Label dummy;
  auto lnf = FBGetLookAndFeelFor(this);
  auto textSize = lnf->GetStringSizeCached(_text);
  auto const& scheme = lnf->FindColorSchemeFor(*this);
  g.setFont(getLookAndFeel().getLabelFont(dummy));
  g.setColour(scheme.text);
  int y = static_cast<int>(std::ceil((getBounds().getHeight() - textSize.y) * 0.5f) + _vOffset);
  g.drawMultiLineText(_text, 3, y, textSize.x, Justification::centredLeft);
}