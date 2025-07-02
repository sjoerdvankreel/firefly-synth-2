#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>

using namespace juce;

FBAutoSizeButton::
FBAutoSizeButton(std::string const& text):
TextButton(),
IFBHorizontalAutoSize(),
_textWidth(FBGUIGetStringWidthCached(text))
{
  setButtonText(text);
}

int 
FBAutoSizeButton::FixedHeight() const
{
  return 20;
}

int 
FBAutoSizeButton::FixedWidth(int /*height*/) const
{
  return _textWidth + 16;
}