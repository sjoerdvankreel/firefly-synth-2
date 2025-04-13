#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Basic, DSF, FM };
enum class FFOsciFMMode { Linear, Exp };
enum class FFOsciFMRatioMode { Ratio, Free };
enum class FFOsciDSFMode { Overtones, Bandwidth };

inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciUnisonMaxCount = 16;
inline int constexpr FFOsciOverSamplingFactor = 2;
inline int constexpr FFOsciOverSamplingTimes = 1 << FFOsciOverSamplingFactor;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;

enum class FFOsciParam { 
  Type, Gain, Note, Cent,
  UnisonCount, UnisonOffset, UnisonRandom, UnisonDetune, UnisonSpread,
  BasicSinOn, BasicSawOn, BasicTriOn, BasicSqrOn,
  BasicSinGain, BasicSawGain, BasicTriGain, BasicSqrGain, BasicSqrPW,
  DSFMode, DSFOvertones, DSFBandwidth, DSFDistance, DSFDecay,
  FMMode, FMRatioMode, FMRatioFree, FMIndex, GLFOToGain, Count };