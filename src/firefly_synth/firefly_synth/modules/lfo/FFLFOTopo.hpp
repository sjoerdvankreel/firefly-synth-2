#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;

inline int constexpr FFLFOMaxSeed = 255;
inline int constexpr FFLFOBlockCount = 3;
std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool global);

enum class FFLFOType { Off, Free, SnapOrOneShot };
enum class FFLFOWaveMode { Off, Sin };
enum class FFLFOOpMode { Add, Mul, Stack };
enum class FFLFOSkewMode { Off, ScaleUnipolar, ScaleBipolar, ExpUnipolar, ExpBipolar };

enum class FFLFOParam {
  Type, Sync, Seed, SmoothTime, SmoothBars,
  SkewXMode, SkewXAmt, SkewYMode, SkewYAmt,
  WaveMode, RateTime, RateBars, Phase, Steps, OpMode, Scale, Count };