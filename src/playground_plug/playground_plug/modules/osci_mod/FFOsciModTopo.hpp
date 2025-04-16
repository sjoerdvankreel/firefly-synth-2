#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>

#include <string>
#include <memory>
#include <utility>

inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount - 1)) / 2;

struct FBStaticModule;
enum class FFOsciModAMMode { Off, AM, RM };
enum class FFOsciModFMMode { PM, FM, Exp };
enum class FFOsciModParam { Oversampling, FMMode, AMMode, AMMix, FMOn, FMIndex, Count };

std::string FFOsciModFormatSlot(int slot);
std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();