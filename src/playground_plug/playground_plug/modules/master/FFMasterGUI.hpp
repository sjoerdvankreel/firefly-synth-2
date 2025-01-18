#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

struct FBRuntimeTopo;
class IFBHostGUIContext;

std::unique_ptr<juce::Component>
FFMakeMasterGUI(FBRuntimeTopo const* topo, IFBHostGUIContext* hostContext);
