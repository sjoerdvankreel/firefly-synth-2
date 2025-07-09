#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;

inline int constexpr FFLFOMaxSeed = 255;
inline int constexpr FFLFOBlockCount = 3;
std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool global);

enum class FFLFOOpType { Off, Add, Mul, Stack };
enum class FFLFOType { Off, Free, SnapOrOneShot };
enum class FFLFOSkewMode { Off, ScaleUnipolar, ScaleBipolar, ExpUnipolar, ExpBipolar };
enum class FFLFOWaveMode { 
  Saw, Tri, Sqr, Random, FreeRandom, Smooth, FreeSmooth,
  Sin, Cos, Sin2, Cos2, SinCos, CosSin,
  Sin3, Cos3, Sn2Cs, Cs2Sn, SnCs2, CsSn2, SnCsSn, CsSnCs };

enum class FFLFOParam {
  Type, Sync, Seed, SmoothTime, SmoothBars,
  SkewXMode, SkewXAmt, SkewYMode, SkewYAmt,
  OpType, Scale, WaveMode, RateHz, RateBars, Phase, Steps, Count };