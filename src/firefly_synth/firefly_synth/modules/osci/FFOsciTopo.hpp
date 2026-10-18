#pragma once

#include <memory>
#include <string>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo(bool isFx);

inline int FFOsciSamplerGUISampleCount = 1024;
inline std::string FFOsciSampleSlotIds[FFOsciCount] = {
  "{1D4FC59F-5B0C-4395-B1EA-9AA50F4D8C0A}",
  "{A64D418D-7434-461D-A9C8-7A7852C64161}",
  "{4FD839F4-96AB-46D8-82B6-A77F6C824065}",
  "{F960C77A-07F3-4BB0-9E20-D98B498DD4CE}" };

enum class FFOsciFMMode { Linear, Exp };
enum class FFOsciStringMode { Uni, Norm };
enum class FFOsciFMRatioMode { Ratio, Free };

enum class FFOsciType { 
  Off, Wave, FM, String, AudioIn, Sample };
std::string FFOsciTypeToString(FFOsciType type);

enum class FFOsciWaveDSFMode { Off, Over, BW };
enum class FFOsciWaveHSMode { Off, Saw, Sqr, Tri };
enum class FFOsciWavePWMode { Off, Sqr, Rect, TriPls, TriSaw, TrapTri, HWSaw };
enum class FFOsciWaveBasicMode { Off, Sin, Cos, Saw, Tri, Rect, Ramp, SinSqr, 
  SawSqr, SinSaw, BSSin, HWSin, FWSin, AltSin, Trap, SawM1, SqrM1, Parabl, HypTri };
enum class FFOsciAudioInSource { AudioIn, Sidechain };

inline int constexpr FFOsciUniMaxCount = 8;
inline int constexpr FFOsciWavePWCount = 2;
inline int constexpr FFOsciWaveBasicCount = 2;
inline int constexpr FFOsciStringMaxPoles = 8;
inline int constexpr FFOsciStringMaxSeed = 255;
inline int constexpr FFOsciStringGraphRounds = 5;
inline int constexpr FFOsciFMRatioCount = 16;
inline int constexpr FFOsciFMOperatorCount = 3;
inline int constexpr FFOsciFMMatrixSize = FFOsciFMOperatorCount * FFOsciFMOperatorCount;

enum class FFOsciParam {
  Type, Phase, KeyTrack, Gain, EnvToGain, Pan, Coarse, Fine, LFOToFine,
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
  AudioInSource,
  AudioInGain, AudioInBal,
  AudioInLPOn, AudioInLPFreq, AudioInLPRes,
  AudioInHPOn, AudioInHPFreq, AudioInHPRes,
  SampleIsLoaded,
  Count };