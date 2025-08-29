#include <firefly_base/gui/controls/FBTextBox.hpp>

using namespace juce;

FBAutoSizeTextBox::
FBAutoSizeTextBox(int fixedWidth):
_fixedWidth(fixedWidth) {}

int 
FBAutoSizeTextBox::FixedHeight() const
{
  return 24;
}

int
FBAutoSizeTextBox::FixedWidth(int /*height*/) const
{
  return _fixedWidth;
}