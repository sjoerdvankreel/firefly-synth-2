#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <playground_plug/shared/FFPlugTopo.hpp>

#include <map>
#include <array>
#include <string>
#include <vector>

class FBPlugGUI;
class FBGridComponent;

inline int constexpr FFOsciModSlotCount = FFOsciCount * FFOsciCount;

std::vector<int>
FFMakeOsciModUIColumnSizes();
juce::Component*
FFMakeOsciModGUI(FBPlugGUI* plugGUI);
void
FFMarkOsciModUIGridSections(FBGridComponent* grid); 

std::string
FFOsciModMakeSourceAndTargetText(int slot);
std::map<std::pair<int, int>, int> const&
FFOsciModSourceAndTargetToSlot();
std::array<std::pair<int, int>, FFOsciModSlotCount> const&
FFOsciModSlotToSourceAndTarget();
