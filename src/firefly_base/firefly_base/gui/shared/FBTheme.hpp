#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <string>

struct FBTheme;
struct FBRuntimeTopo;

// Needs some indirection here and there to prevent use-before-load stuff.
enum class FBThemeResourceId
{
  FolderName,
  FontFileName,
  AboutBoxImageFileName
};

struct FBThemeResources
{
  std::string folderName = {};
  std::string fontFileName = {};
  std::string aboutBoxImageFileName = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBThemeResources);
  std::string GetResourceName(FBThemeResourceId resourceId) const;
};

struct FBColorScheme
{
  float graphAlpha = {};
  juce::Colour text = {};
  juce::Colour text2 = {};
  juce::Colour background = {};
  juce::Colour meterFill = {};
  juce::Colour meterTrack = {};
  juce::Colour graphGrid = {};
  juce::Colour graphBackground = {};
  juce::Colour sectionBorder = {};
  juce::Colour sectionBackground = {};
  juce::Colour primary = {};
  juce::Colour paramSecondary = {};
  juce::Colour paramHighlight = {};
  juce::Colour paramBackground = {};
  juce::Colour sliderEngineThumb = {};
  juce::Colour alertWindowPrimary = {};
  juce::Colour fileBrowserPrimary = {};
  juce::Colour fileBrowserHighlight = {};
  juce::Colour buttonBackground = {};
  juce::Colour activeTabBackground = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBColorScheme);
};

// This is only a struct so we can add "comment" fields in the json.
struct FBComponentColors
{
  std::string colorScheme = {};
};

struct FBModuleColors
{
  std::string colorScheme = {};
  std::map<int, std::string> guiParamColorSchemes = {}; // runtime param index
  std::map<int, std::string> audioParamColorSchemes = {}; // runtime param index
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBModuleColors);
};

struct FBThemeGlobal
{
  int fontSize = {};
  std::string name = {};
  FBThemeResources resources = {};
  FBColorScheme defaultColorScheme = {};
  std::map<std::string, FBColorScheme> colorSchemes = {};
  std::map<std::string, FBComponentColors> componentColors = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBThemeGlobal);
};

struct FBTheme
{
  FBThemeGlobal global = {};
  std::map<int, FBModuleColors> moduleColors = {}; // runtime module index
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBTheme);
};

std::vector<FBTheme>
FBLoadThemes(FBRuntimeTopo const* topo);