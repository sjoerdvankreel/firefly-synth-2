#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
class IFBGUIStore;
class IFBHostGUIContext;

juce::Component&
FFMakeOsciGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  IFBGUIStore* store, IFBHostGUIContext* hostContext);
