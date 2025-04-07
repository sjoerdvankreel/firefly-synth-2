#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Basic, DSF };
enum class FFOsciDSFMode { Overtones, Bandwidth };

inline int constexpr FFOsciUnisonMaxCount = 16;
inline int constexpr FFOsciOverSamplingFactor = 2;
inline int constexpr FFOsciOverSamplingTimes = 1 << FFOsciOverSamplingFactor;

enum class FFOsciParam { 
  Type, Gain, Note, Cent,
  UnisonCount, UnisonOffset, UnisonRandom, UnisonDetune, UnisonSpread,
  BasicSinOn, BasicSawOn, BasicTriOn, BasicSqrOn,
  BasicSinGain, BasicSawGain, BasicTriGain, BasicSqrGain, BasicSqrPW,
  DSFMode, DSFOvertones, DSFBandwidth, DSFDistance, DSFDecay,
  GLFOToGain, Count };