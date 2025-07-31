#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

juce::Component*
FFMakeMixGUI(FBPlugGUI* plugGUI);
juce::Component*
FFMakeMixGUISectionFXToFX(FBPlugGUI* plugGUI, int moduleType, int fxToFXParam);
juce::Component*
FFMakeMixGUISectionAmpBal(FBPlugGUI* plugGUI, int moduleType, int ampParam, int balParam);