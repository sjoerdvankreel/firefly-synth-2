#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <string>

struct FBTheme;
struct FBRuntimeTopo;

struct FBColorScheme
{
  juce::Colour highlight1 = {};
  juce::Colour highlight2 = {};
  juce::Colour background1 = {};
  juce::Colour background2 = {};
  juce::Colour foreground1 = {};
  juce::Colour foreground2 = {};
};

struct FBModuleColors
{
  std::string colorScheme = {};
  std::map<int, std::string> paramColorSchemes = {}; // runtime param index
};

struct FBTheme
{
  std::string name = {};
  std::map<int, FBModuleColors> moduleColors = {}; // runtime module index
  std::map<std::string, FBColorScheme> colorSchemes = {};
};

std::vector<FBTheme>
FBLoadThemes(FBRuntimeTopo const* topo);