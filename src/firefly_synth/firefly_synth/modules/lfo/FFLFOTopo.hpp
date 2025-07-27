#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_synth/dsp/shared/FFTrigFunctions.hpp>
#include <memory>

struct FBStaticModule;

inline int constexpr FFLFOMaxSeed = 255;
inline int constexpr FFLFOBlockCount = 3;
std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool global);

enum class FFLFOCVOutput { Output, Count };
enum class FFLFOOpType { Off, Add, Mul, Stack };
enum class FFLFOType { Off, Free, SnapOrOneShot };
enum class FFLFOSkewYMode { Off, ExpUnipolar, ExpBipolar };
enum class FFLFOSkewXMode { Off, ScaleUnipolar, ScaleBipolar, ExpUnipolar, ExpBipolar };

enum FFLFOWaveMode { 
  FFLFOWaveModeSaw = FFTrigCount, FFLFOWaveModeTri, FFLFOWaveModeSqr, 
  FFLFOWaveModeRandom, FFLFOWaveModeFreeRandom, 
  FFLFOWaveModeSmooth, FFLFOWaveModeFreeSmooth };

enum class FFLFOParam {
  Type, Sync, Seed, SmoothTime, SmoothBars,
  SkewAXMode, SkewAXAmt, SkewAYMode, SkewAYAmt, PhaseB,
  OpType, Min, Max, WaveMode, RateHz, RateBars, Steps, Count };