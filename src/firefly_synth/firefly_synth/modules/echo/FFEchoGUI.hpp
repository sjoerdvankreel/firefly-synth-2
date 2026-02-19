#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

juce::Component*
FFMakeEchoGUI(FBPlugGUI* plugGUI);
juce::Component*
FFMakeEchoDetailGUI(FBPlugGUI* plugGUI, bool global);