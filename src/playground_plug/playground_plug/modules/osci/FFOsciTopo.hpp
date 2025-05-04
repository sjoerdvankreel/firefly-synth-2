#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Basic, DSF, FM };
enum class FFOsciFMRatioMode { Ratio, Free };
enum class FFOsciDSFMode { Overtones, Bandwidth };

// TODO hstri, hssqr, hssaw
enum class FFOsciBasicMode { 
  Off, 
  Sin, Cos, PWRect, Saw, SawM1, PWHWSaw, Ramp, BSSin, HWSin, FWSin, SinSqr, SawSqr, SinSaw, AltSin, Parabl, 
  Tri, HypTri, PWTriSaw, PWTriSqr, Sqr, SqrM1, PWSqr, Trap, PWTrap, Count };

inline int constexpr FFOsciBasicCount = 4;
inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciUniMaxCount = FBSIMDFloatCount * 2;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;

// todo unison->uni
enum class FFOsciParam { 
  Type, Gain, Coarse, Fine,
  UniCount, UniOffset, UniRandom, UniDetune, UniSpread, UniBlend,
  BasicMode, BasicGain, BasicPW, BasicSync,
  DSFMode, DSFOvertones, DSFBandwidth, DSFDistance, DSFDecay,
  FMExp, FMRatioMode, FMRatioRatio, FMRatioFree, FMIndex, Count };