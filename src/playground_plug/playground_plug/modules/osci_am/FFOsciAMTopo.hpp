#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>

#include <array>
#include <memory>
#include <utility>

struct FBStaticModule;
enum class FFOsciAMParam { On, Mix, Ring, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();
inline int constexpr FFOsciAMSlotCount = (FFOsciCount * (FFOsciCount + 1)) / 2;

std::array<std::pair<int, int>, FFOsciAMSlotCount> const&
FFOsciAMSourcesAndTargets();