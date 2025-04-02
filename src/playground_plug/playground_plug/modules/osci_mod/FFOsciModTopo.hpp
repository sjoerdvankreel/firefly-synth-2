#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <memory>
#include <utility>

inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount - 1)) / 2;

struct FBStaticModule;
enum class FFOsciModParam { On, AM, RM, FM, TZFM, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();