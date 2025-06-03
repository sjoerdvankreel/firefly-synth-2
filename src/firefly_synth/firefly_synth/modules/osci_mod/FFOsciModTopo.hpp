#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <juce_core/juce_core.h>

#include <string>
#include <memory>
#include <utility>

inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount - 1)) / 2;

struct FBStaticModule;
enum class FFOsciModAMMode { Off, AM, RM };
enum class FFOsciModParam { Oversample, ExpoFM, AMMode, AMMix, FMOn, FMIndex, Count };

std::string FFOsciModFormatSlot(int slot);
std::string FFOsciModFormatSlotVertical(int slot);
std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();