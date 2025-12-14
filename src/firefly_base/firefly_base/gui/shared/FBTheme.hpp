#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <string>

struct FBTheme;

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
  std::map<std::string, std::string> paramColorSchemes = {};
};

struct FBTheme
{
  std::string name = {};
  std::map<std::string, FBColorScheme> colorSchemes = {};
  std::map<std::string, FBModuleColors> moduleColors = {};
};

std::map<std::string, FBTheme>
FBLoadThemes();