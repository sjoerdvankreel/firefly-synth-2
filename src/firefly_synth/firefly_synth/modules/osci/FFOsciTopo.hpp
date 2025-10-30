#pragma once

#include <memory>
#include <string>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciFMMode { Linear, Exp };
enum class FFOsciStringMode { Uni, Norm };
enum class FFOsciFMRatioMode { Ratio, Free };

enum class FFOsciType { 
  Off, Wave, FM, String, ExtAudio };
std::string FFOsciTypeToString(FFOsciType type);

enum class FFOsciWaveDSFMode { Off, Over, BW };
enum class FFOsciWaveHSMode { Off, Saw, Sqr, Tri };
enum class FFOsciWavePWMode { Off, Sqr, Rect, TriPls, TriSaw, TrapTri, HWSaw };
enum class FFOsciWaveBasicMode { Off, Sin, Cos, Saw, Tri, Rect, Ramp, SinSqr, 
  SawSqr, SinSaw, BSSin, HWSin, FWSin, AltSin, Trap, SawM1, SqrM1, Parabl, HypTri };

inline int constexpr FFOsciUniMaxCount = 8;
inline int constexpr FFOsciWavePWCount = 2;
inline int constexpr FFOsciWaveBasicCount = 2;
inline int constexpr FFOsciStringMaxPoles = 8;
inline int constexpr FFOsciStringMaxSeed = 255;
inline int constexpr FFOsciStringGraphRounds = 5;
inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;
inline float constexpr FFOsciCoarseSemis = 128.0f;

enum class FFOsciParam {
  Type, Phase, Gain, EnvToGain, Pan, Coarse, Fine, LFOToFine,
  UniCount, UniOffset, UniRandom, UniDetune, UniSpread, UniBlend,
  WaveBasicMode, WaveBasicGain, WavePWMode, WavePWGain, WavePWPW,
  WaveHSMode, WaveHSGain, WaveHSPitch,
  WaveDSFMode, WaveDSFGain, WaveDSFOver, WaveDSFBW, WaveDSFDistance, WaveDSFDecay,
  FMMode, FMRatioMode, FMRatioRatio, FMRatioFree, FMIndex,
  StringMode, StringTrackingKey, StringSeed, StringPoles, 
  StringX, StringY, StringColor, StringExcite,
  StringLPOn, StringLPFreq, StringLPRes, StringLPKTrk,
  StringHPOn, StringHPFreq, StringHPRes, StringHPKTrk,
  StringDamp, StringDampKTrk, StringFeedback, StringFeedbackKTrk, 
  ExtAudioInputGain, ExtAudioInputBal,
  ExtAudioLPOn, ExtAudioLPFreq, ExtAudioLPRes,
  ExtAudioHPOn, ExtAudioHPFreq, ExtAudioHPRes,
  Count };