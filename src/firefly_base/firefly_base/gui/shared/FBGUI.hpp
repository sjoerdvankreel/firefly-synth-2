#pragma once

#include <string>
#include <filesystem>

#include <juce_gui_basics/juce_gui_basics.h>

inline int constexpr FBGUIFPS = 30;
inline int constexpr FBGUIFontSize = 13;

class FBLookAndFeel;
enum class FBGUIRenderType { Basic, Full };
enum class FBHighlightTweakMode { Off, Patch, Session, Default };

void FBGUIInit();
void FBGUITerminate();

int FBGUIGetFontHeightInt();
float FBGUIGetFontHeightFloat();
juce::Font const& FBGUIGetFont();
FBLookAndFeel* FBGetLookAndFeel();

int FBGUIGetStringWidthCached(std::string const& text);
juce::Point<int> FBGUIGetStringSizeCached(std::string const& text);