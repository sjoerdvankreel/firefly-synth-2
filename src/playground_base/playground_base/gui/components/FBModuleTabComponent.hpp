#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

struct FBRuntimeTopo;
class FBPlugGUI;
class IFBHostGUIContext;

typedef std::function<juce::Component*(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)>
FBModuleTabFactory;

class FBModuleTabComponent:
public juce::Component
{
  juce::TabbedComponent _tabs;

public:
  void resized() override;
  FBModuleTabComponent(
    FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
    IFBHostGUIContext* hostContext, int moduleIndex, 
    FBModuleTabFactory const& tabFactory);
};