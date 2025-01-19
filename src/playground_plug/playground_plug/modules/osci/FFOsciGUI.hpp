#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
class FBGUIStore;
class IFBHostGUIContext;

juce::Component&
FFMakeOsciGUI(
  FBRuntimeTopo const* topo, 
  FBGUIStore* store, IFBHostGUIContext* hostContext);
