#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class FBPlugGUI;

typedef std::function<juce::Component*(
  FBPlugGUI* plugGUI, int moduleSlot)>
FBModuleTabFactory;

class FBModuleTabComponent:
public juce::Component
{
  juce::TabbedComponent _tabs;

public:
  void resized() override;
  FBModuleTabComponent(
    FBPlugGUI* plugGUI, int moduleIndex, 
    FBModuleTabFactory const& tabFactory);
};