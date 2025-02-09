#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class FBPlugGUI;

typedef std::function<juce::Component*(
  FBPlugGUI* plugGUI, int moduleSlot)>
FBModuleTabFactory;

class FBModuleTabComponent:
public juce::TabbedComponent
{
  FBPlugGUI* const _plugGUI;
  int const _moduleIndex;

public:
  FBModuleTabComponent(
    FBPlugGUI* plugGUI, int moduleIndex, 
    FBModuleTabFactory const& tabFactory);
  
  void currentTabChanged(
    int newCurrentTabIndex, 
    juce::String const& newCurrentTabName);
};