#pragma once

#include <playground_plug/shared/FFOsciMod.hpp>

#include <array>
#include <memory>
#include <utility>

struct FBStaticModule;
enum class FFOsciAMParam { On, Mix, Ring, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciAMTopo();