#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
inline int constexpr FFOsciUniMaxCount = 8;

enum class FFOsciFMMode { Linear, Exp };
enum class FFOsciStringMode { Uni, Norm };
enum class FFOsciFMRatioMode { Ratio, Free };
enum class FFOsciType { Off, Wave, FM, String };

enum class FFOsciWaveDSFMode { Off, Over, BW };
enum class FFOsciWaveHSMode { Off, Saw, Sqr, Tri };
enum class FFOsciWavePWMode { Off, Sqr, Rect, TriPls, TriSaw, TrapTri, HWSaw };
enum class FFOsciWaveBasicMode { Off, Sin, Cos, Saw, Tri, Rect, Ramp, SinSqr, 
  SawSqr, SinSaw, BSSin, HWSin, FWSin, AltSin, Trap, SawM1, SqrM1, Parabl, HypTri };

inline int constexpr FFOsciWavePWCount = 2;
inline int constexpr FFOsciWaveBasicCount = 2;
inline int constexpr FFOsciStringMaxPoles = 8;
inline int constexpr FFOsciStringMaxSeed = 255;
inline int constexpr FFOsciStringGraphRounds = 5;
inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;

enum class FFOsciParam {
  Type, Gain, Coarse, Fine,
  UniCount, UniOffset, UniRandom, UniDetune, UniSpread, UniBlend,
  WaveBasicMode, WaveBasicGain, WavePWMode, WavePWGain, WavePWPW,
  WaveHSMode, WaveHSGain, WaveHSSync,
  WaveDSFMode, WaveDSFGain, WaveDSFOver, WaveDSFBW, WaveDSFDistance, WaveDSFDecay,
  FMMode, FMRatioMode, FMRatioRatio, FMRatioFree, FMIndex,
  StringMode, StringSeed, StringPoles, StringX, StringY, StringColor, StringExcite,
  StringLPOn, StringLPFreq, StringLPRes, StringLPKTrk,
  StringHPOn, StringHPFreq, StringHPRes, StringHPKTrk,
  StringDamp, StringDampKTrk, StringFeedback, StringFeedbackKTrk,
  StringTrackingKey, StringTrackingRange, Count };