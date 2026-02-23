#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

juce::Component*
FFMakeLFOGUI(FBPlugGUI* plugGUI);
juce::Component*
FFMakeLFODetailGUI(FBPlugGUI* plugGUI, bool global, int moduleSlot);