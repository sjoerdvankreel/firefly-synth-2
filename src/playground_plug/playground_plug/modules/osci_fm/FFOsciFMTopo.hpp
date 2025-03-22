#pragma once

#include <playground_plug/shared/FFOsciMod.hpp>

#include <array>
#include <memory>
#include <utility>

struct FBStaticModule;
enum class FFOsciFMMode { Off, On, ThroughZero };
enum class FFOsciFMParam { Mode, Index, Delay, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciFMTopo();