#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;

inline int constexpr FFLFOMaxSeed = 255;
inline int constexpr FFLFOBlockCount = 3;
std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool global);

enum class FFLFOType { Off, Free, SnapOrOneShot };
enum class FFLFOOpType { Off, Add, Mul, Stack, Apply };
enum class FFLFOWaveMode { Sin, Cos };
enum class FFLFOSkewMode { Off, ScaleUnipolar, ScaleBipolar, ExpUnipolar, ExpBipolar };

enum class FFLFOParam {
  Type, Sync, Seed, SmoothTime, SmoothBars,
  SkewXMode, SkewXAmt, SkewYMode, SkewYAmt,
  OpType, Scale, WaveMode, RateTime, RateBars, Phase, Steps, Count };