#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

// TODO cache stuff - procstatecontainer inside the ffpluggui
class FBModuleGraphComponent final:
public juce::Component
{
  int _tweakedParamByUI = -1;
  FBPlugGUI const* const _plugGUI;
  FBModuleGraphComponentData _data = {};

public:
  void RequestRerender(int paramIndex);
  void paint(juce::Graphics& g) override;
  FBModuleGraphComponent(FBPlugGUI const* plugGUI);
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphComponent);
};