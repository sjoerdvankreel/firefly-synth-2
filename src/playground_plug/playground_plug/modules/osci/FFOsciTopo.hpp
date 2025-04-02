#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Basic, DSF };
enum class FFOsciDSFMode { Overtones, Bandwidth };
static inline int constexpr FFOsciUnisonMaxCount = 16;

enum class FFOsciParam { 
  Type, Gain, Note, Cent,
  UnisonCount, UnisonOffset, UnisonRandom, UnisonDetune, UnisonSpread,
  BasicSinOn, BasicSawOn, BasicTriOn, BasicSqrOn,
  BasicSinGain, BasicSawGain, BasicTriGain, BasicSqrGain, BasicSqrPW,
  DSFMode, DSFOvertones, DSFBandwidth, DSFDistance, DSFDecay,
  GLFOToGain, Count };