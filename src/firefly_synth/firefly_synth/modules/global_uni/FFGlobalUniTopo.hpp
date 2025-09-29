#pragma once

#include <memory>

struct FBStaticModule;

inline int constexpr FFGlobalUniMaxCount = 8;
inline int constexpr FFGlobalUniMaxSeed = 255;
inline float constexpr FFGlobalUniCoarseSemis = 36.0f;

enum class FFGlobalUniType {
  Off, Auto, Manual
};

enum class FFGlobalUniTarget {
  VoiceCoarse, VoiceFine, VMixAmp, VMixBal,
  OscGain, OscPan, OscCoarse, OscFine, OscPhaseOffset,
  LFORate, LFOMin, LFOMax, LFOSkewAX, LFOSkewAY, LFOPhaseOffset,
  EnvOffset, EnvStretch, VFXParamA, VFXParamB, VFXParamC, VFXParamD,
  EchoExtend, EchoFade, EchoTapLevel, EchoTapDelay, EchoTapBal,
  EchoFdbkDelay, EchoFdbkMix, EchoFdbkAmt,
  EchoReverbMix, EchoReverbSize, EchoReverbDamp, Count };

// todo spread/space
enum class FFGlobalUniParam { 
  VoiceCount, 
  // by target dimension
  Mode, AutoSpread, AutoSpace, AutoRandom, AutoSeed, AutoReseed,
  // by voice dimension (need different control types per param like bipolar yes/no)
  ManualVoiceCoarse, ManualFirst = ManualVoiceCoarse, ManualVoiceFine, ManualVMixAmp, ManualVMixBal,
  ManualOscGain, ManualOscPan, ManualOscCoarse, ManualOscFine, ManualOscPhaseOffset,
  ManualLFORate, ManualLFOMin, ManualLFOMax, ManualLFOSkewAX, ManualLFOSkewAY, ManualLFOPhaseOffset,
  ManualEnvOffset, ManualEnvStretch, ManualVFXParamA, ManualVFXParamB, ManualVFXParamC, ManualVFXParamD,
  ManualEchoExtend, ManualEchoFade,
  ManualEchoTapLevel, ManualEchoTapDelay, ManualEchoTapBal,
  ManualEchoFdbkDelay, ManualEchoFdbkMix, ManualEchoFdbkAmt,
  ManualEchoReverbMix, ManualEchoReverbSize, ManualEchoReverbDamp, ManualLast = ManualEchoReverbDamp,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
