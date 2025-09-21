#pragma once

#include <memory>

struct FBStaticModule;
inline int constexpr FFGlobalUniMaxCount = 8;

enum class FFGlobalUniType { Off, Basic, Full };
enum class FFGlobalUniParam { 
  Type, Count, 
  FullOscGain, FullFirst = FullOscGain, FullOscPan, FullOscCoarse, FullOscFine, FullOscPhaseOffset, FullOscPhaseRand,
  FullLFORate, FullLFOMin, FullLFOMax, FullLFOSkewAX, FullLFOSkewAY, FullLFOPhaseOffset, FullLFOPhaseRand,
  FullEnvOffset, FullEnvStretch, FullVFXParamA, FullVFXParamB, FullVFXParamC, FullVFXParamD,
  FullEchoExtend, FullEchoFade,
  FullEchoTapLevel, FullEchoTapDelay, FullEchoTapBal,
  FullEchoFdbkDelay, FullEchoFdbkMix, FullEchoFdbkAmt,
  FullEchoReverbMix, FullEchoReverbSize, FullEchoReverbDamp, FullLast = FullEchoReverbDamp,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
