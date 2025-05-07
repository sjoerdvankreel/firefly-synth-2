#pragma once

#include <string>
#include <juce_gui_basics/juce_gui_basics.h>

void FBGUIInit();
void FBGUITerminate();

juce::Font const& FBGUIGetFont();
int FBGUIGetFontHeightInt();
float FBGUIGetFontHeightFloat();
int FBGUIGetStringWidthCached(std::string const& text);
juce::Point<int> FBGUIGetStringSizeCached(std::string const& text);