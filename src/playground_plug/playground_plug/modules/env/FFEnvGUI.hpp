#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class FBPlugGUI;

juce::Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI);
std::unique_ptr<juce::Component>
FFMakeEnvGraphControls(FBPlugGUI* context, int moduleSlot);