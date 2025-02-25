#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Off, Basic, DSF };
enum class FFOsciParam { 
  Type, Gain, Note, Cent,
  BasicSinOn, BasicSawOn, BasicTriOn, BasicSqrOn,
  BasicSinGain, BasicSawGain, BasicTriGain, BasicSqrGain, BasicSqrPW,
  DSFOvertones, DSFDistance, DSFDecay,
  GLFOToGain, Count };