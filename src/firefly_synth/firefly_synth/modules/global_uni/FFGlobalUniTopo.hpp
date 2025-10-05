#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>

#include <memory>

struct FBStaticModule;

inline int constexpr FFGlobalUniMaxCount = 8;
inline int constexpr FFGlobalUniMaxSeed = 255;

enum class FFGlobalUniMode { Off, Auto, Manual };

enum class FFGlobalUniTarget {
  VoiceCoarse, VoiceFine, VMixAmp, VMixBal,
  OscGain, OscPan, OscCoarse, OscFine, OscPhaseOffset,
  LFORate, LFOMin, LFOMax, LFOSkewAX, LFOSkewAY, LFOPhaseOffset,
  EnvSlope, EnvStretch, VFXParamA, VFXParamB, VFXParamC, VFXParamD,
  EchoExtend, EchoFade, EchoTapLevel, EchoTapDelay, EchoTapBal,
  EchoFdbkDelay, EchoFdbkMix, EchoFdbkAmt,
  EchoReverbMix, EchoReverbSize, EchoReverbDamp, Count };

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
  ManualEnvSlope, ManualEnvStretch, ManualVFXParamA, ManualVFXParamB, ManualVFXParamC, ManualVFXParamD,
  ManualEchoExtend, ManualEchoFade,
  ManualEchoTapLevel, ManualEchoTapDelay, ManualEchoTapBal,
  ManualEchoFdbkDelay, ManualEchoFdbkMix, ManualEchoFdbkAmt,
  ManualEchoReverbMix, ManualEchoReverbSize, ManualEchoReverbDamp, ManualLast = ManualEchoReverbDamp,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
