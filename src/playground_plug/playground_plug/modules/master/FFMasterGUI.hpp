#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
class IFBGUIStore;
class IFBHostGUIContext;

juce::Component&
FFMakeMasterGUI(FBRuntimeTopo const* topo, IFBGUIStore* store, IFBHostGUIContext* hostContext);
