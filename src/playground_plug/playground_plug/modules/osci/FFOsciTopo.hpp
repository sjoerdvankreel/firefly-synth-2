#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Wave, DSF, FM };
enum class FFOsciFMRatioMode { Ratio, Free };
enum class FFOsciDSFMode { Overtones, Bandwidth };

enum class FFOsciWaveHSMode {
  Off,
  Saw, Sqr, Tri };

enum class FFOsciWavePWMode {
  Off, 
  Sqr, Rect, 
  TriPls, TriSaw, TrapTri,
  HWSaw };

enum class FFOsciWaveBasicMode {
  Off, 
  Sin, Cos, Saw, Tri, Rect, Ramp, 
  SinSqr, SawSqr, SinSaw,
  BSSin, HWSin, FWSin, AltSin,
  Trap, SawM1, SqrM1, Parabl, HypTri };

inline int constexpr FFOsciWavePWCount = 2;
inline int constexpr FFOsciWaveBasicCount = 2;
inline int constexpr FFOsciUniMaxCount = FBSIMDFloatCount * 2;
inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;

enum class FFOsciParam { 
  Type, Gain, Coarse, Fine,
  UniCount, UniOffset, UniRandom, UniDetune, UniSpread, UniBlend,
  WaveBasicMode, WaveBasicGain, WavePWMode, WavePWGain, WavePWPW, WaveHSMode, WaveHSGain, WaveHSPW, WaveHSSync,
  DSFMode, DSFOvertones, DSFBandwidth, DSFDistance, DSFDecay,
  FMExp, FMRatioMode, FMRatioRatio, FMRatioFree, FMIndex, Count };