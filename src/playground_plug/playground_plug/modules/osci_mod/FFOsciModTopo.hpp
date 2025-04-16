#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>

#include <string>
#include <memory>
#include <utility>

inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount - 1)) / 2;

struct FBStaticModule;
enum class FFOsciModAMMode { Off, AM, RM };
enum class FFOsciModParam { Oversampling, ExpoFM, AMMode, AMMix, FMOn, FMIndexLin, FMIndexExp, Count };

std::string FFOsciModFormatSlot(int slot);
std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();