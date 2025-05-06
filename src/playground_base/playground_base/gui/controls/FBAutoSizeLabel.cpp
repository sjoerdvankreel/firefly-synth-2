#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/controls/FBAutoSizeLabel.hpp>

using namespace juce;

FBAutoSizeLabel::
FBAutoSizeLabel(std::string const& text):
Label(),
IFBHorizontalAutoSize(),
_textWidth(FBGetStringWidthCached(text))
{
  setBorderSize({ 1, 2, 1, 2 });
  setText(text, dontSendNotification);
}

int 
FBAutoSizeLabel::FixedHeight() const
{
  return 20; // TODO
}

int 
FBAutoSizeLabel::FixedWidth(int height) const
{
  return getBorderSize().getLeftAndRight() + _textWidth;
}