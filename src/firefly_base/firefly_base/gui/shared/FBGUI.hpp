#pragma once

#include <string>
#include <filesystem>

#include <juce_gui_basics/juce_gui_basics.h>

inline int constexpr FBGUIFPS = 30;
inline int constexpr FBPrimaryHeight = 24;

class FBPlugGUI;
class FBLookAndFeel;

enum class FBGUIRenderType { Basic, Full };
enum class FBHighlightTweakMode { Off, Patch, Session, Default };

void FBGUIInit();
void FBGUITerminate();
FBLookAndFeel* FBGetLookAndFeelFor(FBPlugGUI const* c);
FBLookAndFeel* FBGetLookAndFeelFor(juce::Component const* c);