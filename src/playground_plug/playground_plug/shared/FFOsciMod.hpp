#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>

#include <array>
#include <string>
#include <vector>

inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount + 1)) / 2;

std::string
FFOsciModMakeSourceAndTargetText(int slot);
std::vector<std::vector<int>> const&
FFOsciModSourceAndTargetToSlot();
std::array<std::pair<int, int>, FFOsciModSlotCount> const&
FFOsciModSlotToSourceAndTarget();