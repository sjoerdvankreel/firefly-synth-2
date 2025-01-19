#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
class FBGUIStore;
class IFBHostGUIContext;

juce::Component&
FFMakeGLFOGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  FBGUIStore* store, IFBHostGUIContext* hostContext);
