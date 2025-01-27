#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

// TODO cache stuff
class FBModuleGraphComponent final:
public juce::Component
{
  int _moduleSlot = -1;
  int _moduleIndex = -1;
  FBPlugGUI const* const _plugGUI;
  FBModuleGraphComponentData _data = {};

public:
  FBModuleGraphComponent(FBPlugGUI const* plugGUI);
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphComponent);
  
  void paint(juce::Graphics& g) override;
  void RequestRerender(int moduleIndex, int moduleSlot);  
};