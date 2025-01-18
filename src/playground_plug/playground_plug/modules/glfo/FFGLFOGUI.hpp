#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
class IFBGUIStore;
class IFBHostGUIContext;

juce::Component&
FFMakeGLFOGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  IFBGUIStore* store, IFBHostGUIContext* hostContext);
