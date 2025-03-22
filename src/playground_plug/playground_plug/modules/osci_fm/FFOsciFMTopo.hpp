#pragma once

#include <playground_plug/shared/FFOsciMod.hpp>

#include <array>
#include <memory>
#include <utility>

struct FBStaticModule;
enum class FFOsciFMParam { On, ThroughZero, Index, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciFMTopo();