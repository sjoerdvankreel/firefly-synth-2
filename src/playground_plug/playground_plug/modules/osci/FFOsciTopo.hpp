#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Wave, DSF, FM };
enum class FFOsciFMRatioMode { Ratio, Free };
enum class FFOsciDSFMode { Overtones, Bandwidth };

// TODO hstri, hssqr, hssaw
enum class FFOsciWaveMode { 
  Off, 
  Sin, Cos, Saw, Ramp, Sqr, Tri, Trap, SinSqr, SawSqr, SinSaw,
  SawM1, SqrM1, BSSin, HWSin, FWSin, AltSin, Parabl, HypTri, 
  PWRect, PWSqr, PWHWSaw, PWTriSaw, PWTriPls, PWTrapTri, Count };

inline int constexpr FFOsciWaveCount = 4;
inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciUniMaxCount = FBSIMDFloatCount * 2;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;

enum class FFOsciParam { 
  Type, Gain, Coarse, Fine,
  UniCount, UniOffset, UniRandom, UniDetune, UniSpread, UniBlend,
  WaveMode, WaveGain, WavePW, WaveSync,
  DSFMode, DSFOvertones, DSFBandwidth, DSFDistance, DSFDecay,
  FMExp, FMRatioMode, FMRatioRatio, FMRatioFree, FMIndex, Count };