#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;

inline int constexpr FFLFOMaxSeed = 255;
inline int constexpr FFLFOBlockCount = 1; // TODO
std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool global);

enum class FFLFOOpMode { Add, Mul };
enum class FFLFOWaveMode { Off, Sin };
enum class FFLFOSkewMode { Off, ScaleUnipolar, ScaleBipolar, ExpUnipolar, ExpBipolar };

enum class FFLFOParam {
  On, Sync, OneShot, HostSnap, 
  Phase, Steps, Seed, SmoothTime, SmoothBars,
  WaveMode, RateTime, RateBars, OpMode, Stack, Min, Max, 
  SkewXMode, SkewXAmt, SkewYMode, SkewYAmt, Count };