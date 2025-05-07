#include <playground_base/gui/shared/FBGUIConfig.hpp>
#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/controls/FBAutoSizeMultiLineLabel.hpp>

using namespace juce;

FBAutoSizeMultiLineLabel::
FBAutoSizeMultiLineLabel(juce::String const& text):
Component(),
IFBHorizontalAutoSize(),
_text(text),
_textSize(FBGUIGetStringSizeCached(text.toStdString()))
{
}

int 
FBAutoSizeMultiLineLabel::FixedWidth(int height) const
{
  return _textSize.x + 2;
}

void
FBAutoSizeMultiLineLabel::paint(Graphics& g)
{
  Label dummy;
  g.setFont(getLookAndFeel().getLabelFont(dummy));
  g.setColour(getLookAndFeel().findColour(Label::ColourIds::textColourId));
  float y = (getBounds().getHeight() - _textSize.y) * 0.5f - 2.0f;
  g.drawMultiLineText(_text, 1, y, _textSize.x, Justification::centred);
}