#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <string>

struct FBTheme;
struct FBRuntimeTopo;

struct FBColorScheme
{
  float graphAlpha = {};
  float dimDisabled = {};
  juce::Colour text = {};
  juce::Colour background = {};
  juce::Colour graphGrid = {};
  juce::Colour graphBackground = {};
  juce::Colour sectionBorder = {};
  juce::Colour sectionBackground = {};
  juce::Colour primary = {};
  juce::Colour paramSecondary = {};
  juce::Colour paramHighlight = {};
  juce::Colour paramBackground = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBColorScheme);
};

struct FBModuleColors
{
  std::string colorScheme = {};
  std::map<int, std::string> guiParamColorSchemes = {}; // runtime param index
  std::map<int, std::string> audioParamColorSchemes = {}; // runtime param index
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBModuleColors);
};

struct FBTheme
{
  std::string name = {};
  FBColorScheme defaultColorScheme = {};
  std::map<int, FBModuleColors> moduleColors = {}; // runtime module index
  std::map<std::string, FBColorScheme> colorSchemes = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBTheme);
};

std::vector<FBTheme>
FBLoadThemes(FBRuntimeTopo const* topo);