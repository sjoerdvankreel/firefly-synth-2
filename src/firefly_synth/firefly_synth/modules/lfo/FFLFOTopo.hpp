#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_synth/dsp/shared/FFTrigFunctions.hpp>
#include <memory>

struct FBStaticModule;

inline int constexpr FFLFOMaxSeed = 255;
inline int constexpr FFLFOBlockCount = 3;
std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool global);

enum class FFLFOOpType { Off, Add, Mul, Stack };
enum class FFLFOType { Off, Free, SnapOrOneShot };
enum class FFLFOSkewMode { Off, ScaleUnipolar, ScaleBipolar, ExpUnipolar, ExpBipolar };
enum FFLFOWaveMode { 
  FFLFOWaveModeSaw = FFTrigCount, FFLFOWaveModeTri, FFLFOWaveModeSqr, 
  FFLFOWaveModeRandom, FFLFOWaveModeFreeRandom, 
  FFLFOWaveModeSmooth, FFLFOWaveModeFreeSmooth };

enum class FFLFOParam {
  Type, Sync, Seed, Phase, SmoothTime, SmoothBars,
  SkewXMode, SkewXAmt, SkewYMode, SkewYAmt,
  OpType, Min, Max, WaveMode, RateHz, RateBars, Steps, Count };