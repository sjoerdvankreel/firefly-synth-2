#pragma once

#include <memory>

struct FBStaticModule;

inline int constexpr FFGlobalUniMaxCount = 8;
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
  TypeVoiceCoarse, TypeFirst = TypeVoiceCoarse, TypeVoiceFine, TypeVMixAmp, TypeVMixBal,
  TypeOscGain, TypeOscPan, TypeOscCoarse, TypeOscFine, TypeOscPhaseOffset,
  TypeLFORate, TypeLFOMin, TypeLFOMax, TypeLFOSkewAX, TypeLFOSkewAY, TypeLFOPhaseOffset,
  TypeEnvOffset, TypeEnvStretch, TypeVFXParamA, TypeVFXParamB, TypeVFXParamC, TypeVFXParamD,
  TypeEchoExtend, TypeEchoFade,
  TypeEchoTapLevel, TypeEchoTapDelay, TypeEchoTapBal,
  TypeEchoFdbkDelay, TypeEchoFdbkMix, TypeEchoFdbkAmt,
  TypeEchoReverbMix, TypeEchoReverbSize, TypeEchoReverbDamp, TypeLast = TypeEchoReverbDamp,
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
