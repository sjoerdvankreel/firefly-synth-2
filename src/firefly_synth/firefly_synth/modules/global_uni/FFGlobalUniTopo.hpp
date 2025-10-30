#pragma once

#include <memory>

struct FBStaticModule;

inline int constexpr FFGlobalUniMaxCount = 8;
inline int constexpr FFGlobalUniMaxSeed = 255;

enum class FFGlobalUniMode { Off, Manual, AutoLinear, AutoExp };

enum class FFGlobalUniTarget {
  VoiceCoarse, VoiceFine, VMixAmp, VMixBal,
  OscGain, OscPan, OscCoarse, OscFine, OscPhaseOffset,
  LFORate, LFOMin, LFOMax, LFOSkewAX, LFOSkewAY, LFOPhaseOffset,
  VFXParamA, VFXParamB, VFXParamC, VFXParamD,
  EnvSlope, EnvStretch,
  EchoExtend, EchoFade, 
  EchoTapMix, EchoTapDelay, EchoTapBal, EchoTapLPF, EchoTapHPF,
  EchoFdbkDelay, EchoFdbkMix, EchoFdbkAmt, EchoFdbkLPF, EchoFdbkHPF,
  EchoReverbMix, EchoReverbSize, EchoReverbDamp, EchoReverbLPF, EchoReverbHPF, Count };

FFModuleType
FFGlobalUniTargetToModule(FFGlobalUniTarget target);
std::string
FFGlobalUniTargetToString(FFGlobalUniTarget target);
float
FFGlobalUniTargetGetDefaultValue(FFGlobalUniTarget target);

enum class FFGlobalUniParam { 
  VoiceCount, 
  // by target dimension
  Mode, OpType, AutoSpread, AutoFirst = AutoSpread, AutoSkew, AutoRand, AutoRandSeed, AutoRandFree, AutoLast = AutoRandFree,
  // by voice dimension (need different control types per param like bipolar yes/no)
  ManualVoiceCoarse, ManualFirst = ManualVoiceCoarse, ManualVoiceFine, ManualVMixAmp, ManualVMixBal,
  ManualOscGain, ManualOscPan, ManualOscCoarse, ManualOscFine, ManualOscPhaseOffset,
  ManualLFORate, ManualLFOMin, ManualLFOMax, ManualLFOSkewAX, ManualLFOSkewAY, ManualLFOPhaseOffset,
  ManualVFXParamA, ManualVFXParamB, ManualVFXParamC, ManualVFXParamD,
  ManualEnvSlope, ManualEnvStretch,
  ManualEchoExtend, ManualEchoFade,
  ManualEchoTapMix, ManualEchoTapDelay, ManualEchoTapBal, ManualEchoTapLPF, ManualEchoTapHPF,
  ManualEchoFdbkDelay, ManualEchoFdbkMix, ManualEchoFdbkAmt, ManualEchoFdbkLPF, ManualEchoFdbkHPF,
  ManualEchoReverbMix, ManualEchoReverbSize, ManualEchoReverbDamp, ManualEchoReverbLPF, ManualEchoReverbHPF,
  ManualLast = ManualEchoReverbHPF,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
