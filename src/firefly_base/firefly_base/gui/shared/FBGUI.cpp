#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <filesystem>
#include <unordered_map>

using namespace juce;

static Font _font = {};
static Typeface::Ptr _typeface = {};
static std::unique_ptr<FBLookAndFeel> _lookAndFeel = {};

Font const& FBGUIGetFont() { return _font; }
LookAndFeel* FBGetLookAndFeel() { return _lookAndFeel.get(); }
float FBGUIGetFontHeightFloat() { return FBGUIGetFont().getHeight(); }
int FBGUIGetFontHeightInt() { return static_cast<int>(std::ceil(FBGUIGetFontHeightFloat())); }
int FBGUIGetStringWidthCached(std::string const& text) { return FBGUIGetStringSizeCached(text).x; }

void
FBGUITerminate()
{
  FB_LOG_INFO("Terminating GUI.");
  LookAndFeel::setDefaultLookAndFeel(nullptr);
  _lookAndFeel.reset();
  _font = {};
  _typeface.reset();
  FB_LOG_INFO("Terminating JUCE GUI.");
  shutdownJuce_GUI();
  FB_LOG_INFO("Terminated JUCE GUI.");
  FB_LOG_INFO("Terminated GUI.");
}

void
FBGUIInit()
{
  FB_LOG_INFO("Initializing GUI.");
  FB_LOG_INFO("Initializing JUCE GUI.");
  initialiseJuce_GUI();
  FB_LOG_INFO("Initialized JUCE GUI.");
  File selfJuce(File::getSpecialLocation(File::currentExecutableFile));
  std::filesystem::path selfPath(selfJuce.getFullPathName().toStdString());
  auto fontPath = selfPath.parent_path().parent_path() / "Resources" / "ui" / "JetBrainsMono-Medium.ttf";
  auto fontBytes = FBReadFile(fontPath);
  _typeface = Typeface::createSystemTypefaceFor(fontBytes.data(), fontBytes.size());
  _font = Font(FontOptions(_typeface)).withHeight(FBGUIFontSize);
  _lookAndFeel = std::make_unique<FBLookAndFeel>();
  LookAndFeel::setDefaultLookAndFeel(_lookAndFeel.get());
  FB_LOG_INFO("Initialized GUI.");
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