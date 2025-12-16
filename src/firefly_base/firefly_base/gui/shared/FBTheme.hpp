#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <string>

struct FBTheme;
struct FBRuntimeTopo;

struct FBColorScheme
{
  juce::Colour border = {};
  juce::Colour background = {};
  juce::Colour foreground = {};
  juce::Colour controlBounds = {};
  juce::Colour controlEngine = {};
  juce::Colour controlTweaked = {};
  juce::Colour controlBorder = {};
  juce::Colour controlBackground = {};
  juce::Colour controlForeground = {};
};

struct FBModuleColors
{
  std::string colorScheme = {};
  std::map<int, std::string> guiParamColorSchemes = {}; // runtime param index
  std::map<int, std::string> audioParamColorSchemes = {}; // runtime param index
};

struct FBTheme
{
  std::string name = {};
  FBColorScheme defaultColorScheme = {};
  std::map<int, FBModuleColors> moduleColors = {}; // runtime module index
  std::map<std::string, FBColorScheme> colorSchemes = {};
};

std::vector<FBTheme>
FBLoadThemes(FBRuntimeTopo const* topo);