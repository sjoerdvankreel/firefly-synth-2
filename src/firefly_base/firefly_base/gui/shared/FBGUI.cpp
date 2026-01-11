#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <cmath>
#include <filesystem>
#include <unordered_map>

using namespace juce;

static Typeface::Ptr _typeface = {};
static Font _font = Font(FontOptions());
static std::unique_ptr<FBLookAndFeel> _defaultLookAndFeel = {};

Font const& FBGUIGetFont() { return _font; }
float FBGUIGetPopupMenuFontHeightFloat() { return 13.0f; }
float FBGUIGetFontHeightFloat() { return FBGUIGetFont().getHeight(); }
int FBGUIGetStandardPopupMenuItemHeight() { return FBGUIGetFontHeightInt() + 10; }
int FBGUIGetFontHeightInt() { return static_cast<int>(std::ceil(FBGUIGetFontHeightFloat())); }
int FBGUIGetStringWidthCached(std::string const& text) { return FBGUIGetStringSizeCached(text).x; }

FBLookAndFeel* 
FBGetLookAndFeelFor(Component const* c)
{
  if (auto container = dynamic_cast<FBPlugGUI const*>(c))
    return container->LookAndFeel();
  if (auto container = c->findParentComponentOfClass<FBPlugGUI>())
    return container->LookAndFeel();
  FB_ASSERT(false);
  return nullptr;
}

void
FBGUITerminate()
{
  FB_LOG_INFO("Terminating GUI.");
  LookAndFeel::setDefaultLookAndFeel(nullptr);
  _defaultLookAndFeel.reset();
  _font = Font(FontOptions());
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
  auto fontPath = FBGetResourcesFolderPath() / "ui" / "JetBrainsMono-Medium.ttf";
  auto fontBytes = FBReadFile(fontPath);
  _typeface = Typeface::createSystemTypefaceFor(fontBytes.data(), fontBytes.size());
  _font = Font(FontOptions(_typeface)).withHeight(FBGUIFontSize);

  // We still need this even with per-pluggui look and feel.
  // It controls some stuff during setting up the gui like createComboBoxTextBox
  // which is fortunately not theme-dependent.
  _defaultLookAndFeel = std::make_unique<FBLookAndFeel>();
  LookAndFeel::setDefaultLookAndFeel(_defaultLookAndFeel.get());
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