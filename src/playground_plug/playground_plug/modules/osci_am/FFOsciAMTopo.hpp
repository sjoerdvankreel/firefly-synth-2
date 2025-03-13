#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <memory>

inline int constexpr FFOsciAMSlotCount = (FFOsciCount * (FFOsciCount + 1)) / 2;

struct FBStaticModule;
enum class FFOsciAMParam { On, Mix, Ring, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();