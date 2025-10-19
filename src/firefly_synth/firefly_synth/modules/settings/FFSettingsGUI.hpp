#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FBPlugGUI;
class FBGraphRenderState;
class FBModuleGraphComponent;

juce::Component*
FFMakeSettingsGUI(
  FBPlugGUI* plugGUI,
  FBGraphRenderState* graphRenderState,
  std::vector<FBModuleGraphComponent*>* fixedGraphs);