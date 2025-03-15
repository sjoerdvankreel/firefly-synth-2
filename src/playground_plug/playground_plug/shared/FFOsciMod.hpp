#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>

#include <map>
#include <array>
#include <string>

inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount + 1)) / 2;

std::string
FFOsciModMakeSourceAndTargetText(int slot);
std::map<std::pair<int, int>, int> const&
FFOsciModSourceAndTargetToSlot();
std::array<std::pair<int, int>, FFOsciModSlotCount> const&
FFOsciModSlotToSourceAndTarget();