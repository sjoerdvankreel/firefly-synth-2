#include <playground_base/gui/shared/FBGUIConfig.hpp>
#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/shared/FBGUILookAndFeel.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <filesystem>
#include <unordered_map>

using namespace juce;

static inline int constexpr FontSize = 13;

static Font _font = {};
static Typeface::Ptr _typeface = {};
static std::unique_ptr<FBGUILookAndFeel> _lookAndFeel = {};

Font const&
FBGUIGetFont()
{
  return _font;
}

float 
FBGUIGetFontHeightFloat()
{
  return FBGUIGetFont().getHeight();
}

int
FBGUIGetFontHeightInt()
{
  return static_cast<int>(std::ceil(FBGUIGetFontHeightFloat()));
}

int
FBGUIGetStringWidthCached(std::string const& text)
{
  return FBGUIGetStringSizeCached(text).x;
}

void
FBGUITerminate()
{
  LookAndFeel::setDefaultLookAndFeel(nullptr);
  _lookAndFeel.reset();
  _font = {};
  _typeface.reset();
  shutdownJuce_GUI();
}

void
FBGUIInit()
{
  initialiseJuce_GUI();
  File selfJuce(File::getSpecialLocation(File::currentExecutableFile));
  std::filesystem::path selfPath(selfJuce.getFullPathName().toStdString());
  auto fontPath = selfPath.parent_path().parent_path() / "Resources" / "ui" / "JetBrainsMono-Medium.ttf";
  auto fontBytes = FBReadFile(fontPath);
  _typeface = Typeface::createSystemTypefaceFor(fontBytes.data(), fontBytes.size());
  _font = Font(FontOptions(_typeface)).withHeight(FontSize);
  _lookAndFeel = std::make_unique<FBGUILookAndFeel>();
  LookAndFeel::setDefaultLookAndFeel(_lookAndFeel.get());
}

juce::Point<int>
FBGUIGetStringSizeCached(std::string const& text)
{
  static std::unordered_map<std::string, juce::Point<int>> cache = {};
  auto iter = cache.find(text);
  if (iter != cache.end())
    return iter->second;
  auto bounds = TextLayout::getStringBounds(_font, text);
  auto result = juce::Point<int>(
    static_cast<int>(std::ceil(bounds.getWidth())), 
    static_cast<int>(std::ceil(bounds.getHeight())));
  cache[text] = result;
  return result;
}