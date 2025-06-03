#pragma once

#include <string>
#include <juce_gui_basics/juce_gui_basics.h>

inline int constexpr FBGUIFPS = 30;
inline int constexpr FBGUIFontSize = 13;
enum class FBGUIRenderType { Basic, Full };

void FBGUIInit();
void FBGUITerminate();

int FBGUIGetFontHeightInt();
float FBGUIGetFontHeightFloat();
juce::Font const& FBGUIGetFont();
juce::LookAndFeel* FBGetLookAndFeel();

int FBGUIGetStringWidthCached(std::string const& text);
juce::Point<int> FBGUIGetStringSizeCached(std::string const& text);