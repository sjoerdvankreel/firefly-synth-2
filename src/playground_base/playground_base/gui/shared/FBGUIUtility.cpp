#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <unordered_map>

using namespace juce;

int
FBGetStringWidthCached(std::string const& text)
{
  static std::unordered_map<std::string, int> cache;
  int& result = cache[text];
  if (result != 0)
    return result;
  result = (int)std::ceil(TextLayout::getStringWidth(Font(12.0f), text));
  return result;
}