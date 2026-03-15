#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

juce::Component*
FFMakeSettingsGUIMain(FBPlugGUI* plugGUI);
juce::Component*
FFMakeSettingsGUITuning(FBPlugGUI* plugGUI);