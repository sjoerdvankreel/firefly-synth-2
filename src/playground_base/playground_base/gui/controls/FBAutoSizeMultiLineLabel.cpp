#include <playground_base/gui/shared/FBGUIConfig.hpp>
#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/controls/FBAutoSizeMultiLineLabel.hpp>

using namespace juce;

FBAutoSizeMultiLineLabel::
FBAutoSizeMultiLineLabel(std::string const& text):
Component(),
IFBHorizontalAutoSize(),
_text(text),
_textWidth(FBGetStringWidthCached(text))
{
}

int 
FBAutoSizeMultiLineLabel::FixedWidth(int height) const
{
  return _textWidth;
}

void
FBAutoSizeMultiLineLabel::paint(Graphics& g)
{
  Label dummy;
  g.setFont(getLookAndFeel().getLabelFont(dummy));
  g.setColour(getLookAndFeel().findColour(Label::ColourIds::textColourId));
  g.drawMultiLineText(_text, 0, FBGUIFontSize, _textWidth, Justification::centred);
}