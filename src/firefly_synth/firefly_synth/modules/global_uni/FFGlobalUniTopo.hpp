#pragma once

#include <memory>

struct FBStaticModule;

inline int constexpr FFGlobalUniMaxCount = 8;
inline float constexpr FFGlobalUniCoarseSemis = 36.0f;

enum class FFGlobalUniType { Off, Basic, Full };
enum class FFGlobalUniTarget {
  VoiceCoarse, VoiceFine, VMixAmp, VMixBal,
  OscGain, OscPan, OscCoarse, OscFine, OscPhaseOffset,
  LFORate, LFOMin, LFOMax, LFOSkewAX, LFOSkewAY, LFOPhaseOffset,
  EnvOffset, EnvStretch, VFXParamA, VFXParamB, VFXParamC, VFXParamD,
  EchoExtend, EchoFade, EchoTapLevel, EchoTapDelay, EchoTapBal,
  EchoFdbkDelay, EchoFdbkMix, FullEchoFdbkAmt,
  EchoReverbMix, EchoReverbSize, FullEchoReverbDamp };

enum class FFGlobalUniParam { 
  Type, VoiceCount, 
  FullVoiceCoarse, FullFirst = FullVoiceCoarse, FullVoiceFine, FullVMixAmp, FullVMixBal,
  FullOscGain, FullOscPan, FullOscCoarse, FullOscFine, FullOscPhaseOffset,
  FullLFORate, FullLFOMin, FullLFOMax, FullLFOSkewAX, FullLFOSkewAY, FullLFOPhaseOffset,
  FullEnvOffset, FullEnvStretch, FullVFXParamA, FullVFXParamB, FullVFXParamC, FullVFXParamD,
  FullEchoExtend, FullEchoFade,
  FullEchoTapLevel, FullEchoTapDelay, FullEchoTapBal,
  FullEchoFdbkDelay, FullEchoFdbkMix, FullEchoFdbkAmt,
  FullEchoReverbMix, FullEchoReverbSize, FullEchoReverbDamp, FullLast = FullEchoReverbDamp,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
