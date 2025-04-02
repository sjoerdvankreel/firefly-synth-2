#pragma once

#include <memory>
#include <utility>

// TODO rename
inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount - 1)) / 2;

struct FBStaticModule;
enum class FFOsciModParam { On, AM, RM, FM, TZFM, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();