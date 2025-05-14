#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeNoiseTopo();

inline int constexpr FFNoiseMaxQ = 16;
inline int constexpr FFNoiseMaxSeed = 255;
inline int constexpr FFNoiseUniMaxCount = FBSIMDFloatCount * 2;

enum class FFNoiseParam { 
  On, Gain, Coarse, Fine,
  UniCount, UniDetune, UniSpread, UniBlend,
  Seed, Q, A, X, Y, Count };