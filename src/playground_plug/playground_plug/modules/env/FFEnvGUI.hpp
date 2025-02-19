#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class FBPlugGUI;

juce::Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI);
juce::Component*
FFMakeEnvGraphControls(FBPlugGUI* plugGUI, int moduleSlot);