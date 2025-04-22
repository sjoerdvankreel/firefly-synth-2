#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Basic, DSF, FM };
enum class FFOsciFMRatioMode { Ratio, Free };
enum class FFOsciDSFMode { Overtones, Bandwidth };

inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciUnisonMaxCount = FBSIMDFloatCount * 4;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;

enum class FFOsciParam { 
  Type, Gain, Coarse, Fine, Sync,
  UnisonCount, UnisonOffset, UnisonRandom, UnisonDetune, UnisonSpread, UnisonBlend,
  BasicSinOn, BasicSawOn, BasicTriOn, BasicSqrOn,
  BasicSinGain, BasicSawGain, BasicTriGain, BasicSqrGain, BasicSqrPW,
  DSFMode, DSFOvertones, DSFBandwidth, DSFDistance, DSFDecay,
  FMExp, FMRatioMode, FMRatioRatio, FMRatioFree, FMIndex, GLFOToGain, Count };