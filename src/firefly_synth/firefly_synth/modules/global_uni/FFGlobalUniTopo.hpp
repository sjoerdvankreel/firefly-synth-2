#pragma once

#include <memory>

struct FBStaticModule;

inline int constexpr FFGlobalUniMaxCount = 8;
inline float constexpr FFGlobalUniOscCoarseSemis = 12.0f;

enum class FFGlobalUniType { Off, Basic, Full };
enum class FFGlobalUniTarget {
  OscGain, OscPan, OscCoarse, OscFine, OscPhaseOffset, OscPhaseRand,
  LFORate, LFOMin, LFOMax, LFOSkewAX, LFOSkewAY, LFOPhaseOffset, LFOPhaseRand,
  EnvOffset, EnvStretch, VFXParamA, VFXParamB, VFXParamC, VFXParamD,
  EchoExtend, EchoFade, EchoTapLevel, EchoTapDelay, EchoTapBal,
  EchoFdbkDelay, EchoFdbkMix, FullEchoFdbkAmt,
  EchoReverbMix, EchoReverbSize, FullEchoReverbDamp };

enum class FFGlobalUniParam { 
  Type, VoiceCount, 
  FullOscGain, FullFirst = FullOscGain, FullOscPan, FullOscCoarse, FullOscFine, FullOscPhaseOffset, FullOscPhaseRand,
  FullLFORate, FullLFOMin, FullLFOMax, FullLFOSkewAX, FullLFOSkewAY, FullLFOPhaseOffset, FullLFOPhaseRand,
  FullEnvOffset, FullEnvStretch, FullVFXParamA, FullVFXParamB, FullVFXParamC, FullVFXParamD,
  FullEchoExtend, FullEchoFade,
  FullEchoTapLevel, FullEchoTapDelay, FullEchoTapBal,
  FullEchoFdbkDelay, FullEchoFdbkMix, FullEchoFdbkAmt,
  FullEchoReverbMix, FullEchoReverbSize, FullEchoReverbDamp, FullLast = FullEchoReverbDamp,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
