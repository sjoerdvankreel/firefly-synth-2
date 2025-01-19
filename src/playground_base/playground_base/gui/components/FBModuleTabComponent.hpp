#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

struct FBRuntimeTopo;
class FBGUIStore;
class IFBHostGUIContext;

typedef std::function<juce::Component&(
  FBRuntimeTopo const* topo, FBGUIStore* store, 
  IFBHostGUIContext* hostContext, int moduleSlot)>
FBModuleTabFactory;

class FBModuleTabComponent:
public juce::Component
{
  juce::TabbedComponent _tabs;

public:
  void resized() override;
  FBModuleTabComponent(
    FBRuntimeTopo const* topo, FBGUIStore* store, 
    IFBHostGUIContext* hostContext, int moduleIndex,
    FBModuleTabFactory const& tabFactory);
};