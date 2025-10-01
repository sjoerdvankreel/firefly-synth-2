#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
class FBGraphRenderState;

juce::Component*
FFMakeGlobalUniGUI(FBPlugGUI* plugGUI, FBGraphRenderState* graphRenderState);
