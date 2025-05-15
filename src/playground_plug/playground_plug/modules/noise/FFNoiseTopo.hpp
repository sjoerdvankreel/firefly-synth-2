#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_plug/modules/osci_base/FFOsciTopoBase.hpp>

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeNoiseTopo();

inline int constexpr FFNoiseMaxPoles = 8;
inline int constexpr FFNoiseMaxSeed = 255;

enum class FFNoiseParam { 
  On, Gain, Coarse, Fine,
  UniCount, UniDetune, UniSpread, UniBlend,
  Seed, Poles, Color, X, Y, Count };