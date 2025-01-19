#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeTopo;
class FBPlugGUI;
class IFBHostGUIContext;

juce::Component&
FFMakeGFilterGUI(
  FBRuntimeTopo const* topo, 
  FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext);