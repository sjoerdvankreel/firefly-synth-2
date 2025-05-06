#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/shared/FBGUILookAndFeel.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <unordered_map>

using namespace juce;

static std::unique_ptr<FBGUILookAndFeel> _lookAndFeel = {};

void 
FBGUIInit()
{
  _lookAndFeel = std::make_unique<FBGUILookAndFeel>();
  LookAndFeel::setDefaultLookAndFeel(_lookAndFeel.get());
}

void 
FBGUITerminate()
{
  LookAndFeel::setDefaultLookAndFeel(nullptr);
  _lookAndFeel.reset();
}

int
FBGetStringWidthCached(std::string const& text)
{
  static std::unordered_map<std::string, int> cache;
  int& result = cache[text];
  if (result != 0)
    return result;
  result = static_cast<int>(std::ceil(TextLayout::getStringWidth(Font(15.0f), text)));
  return result;
}