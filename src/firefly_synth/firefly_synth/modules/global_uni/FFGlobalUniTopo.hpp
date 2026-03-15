#pragma once

#include <memory>

struct FBRuntimeTopo;
struct FBStaticModule;

inline int constexpr FFGlobalUniMaxCount = 8;
inline int constexpr FFGlobalUniMaxSeed = 255;

enum class FFGlobalUniMode { Off, Manual, AutoLinear, AutoExp };

// Don't reorder, that's a breaking change.
// Rely on below FFGlobalUniTargetGUIOrder instead.
enum class FFGlobalUniTarget {
  VoiceCoarse, VoiceFine, VMixAmp, VMixBal,
  OscGain, OscPan, OscCoarse, OscFine, OscPhaseOffset,
  LFORate, LFOMin, LFOMax, LFOPhaseOffset,
  VFXParamA, VFXParamB, VFXParamC, VFXParamD,
  EnvSlope, EnvStretch,
  EchoTapsMix, EchoTapDelay, EchoTapBal,
  EchoFdbkDelay, EchoFdbkMix, EchoFdbkAmt,
  EchoReverbMix, EchoReverbSize, EchoReverbDamp, Count };

enum class FFGlobalUniTargetGUIOrder {
  VoiceCoarse, VoiceFine, VMixAmp, VMixBal,
  LFORate, LFOMin, LFOMax, LFOPhaseOffset,
  EnvSlope, EnvStretch,
  OscGain, OscPan, OscCoarse, OscFine, OscPhaseOffset,
  VFXParamA, VFXParamB, VFXParamC, VFXParamD,
  EchoTapsMix, EchoTapDelay, EchoTapBal,
  EchoFdbkDelay, EchoFdbkMix, EchoFdbkAmt,
  EchoReverbMix, EchoReverbSize, EchoReverbDamp, Count
};

FFModuleType
FFGlobalUniTargetToModule(FFGlobalUniTarget target);
FFGlobalUniTarget
FFGlobalUniTargetByGUIOrder(FFGlobalUniTargetGUIOrder order);
std::string
FFGlobalUniTargetToString(FFGlobalUniTarget target);
float
FFGlobalUniTargetGetDefaultValue(FFGlobalUniTarget target);
FFGlobalUniTarget
FFParamToGlobalUniTarget(FBRuntimeTopo const* topo, int index);

enum class FFGlobalUniParam { 
  VoiceCount, 
  // by target dimension
  Mode, OpType, AutoSpread, AutoFirst = AutoSpread, AutoSkew, AutoRand, AutoRandSeed, AutoRandFree, AutoLast = AutoRandFree,
  // by voice dimension (need different control types per param like bipolar yes/no)
  ManualVoiceCoarse, ManualFirst = ManualVoiceCoarse, ManualVoiceFine, ManualVMixAmp, ManualVMixBal,
  ManualOscGain, ManualOscPan, ManualOscCoarse, ManualOscFine, ManualOscPhaseOffset,
  ManualLFORate, ManualLFOMin, ManualLFOMax, ManualLFOPhaseOffset,
  ManualVFXParamA, ManualVFXParamB, ManualVFXParamC, ManualVFXParamD,
  ManualEnvSlope, ManualEnvStretch,
  ManualEchoTapsMix, ManualEchoTapDelay, ManualEchoTapBal,
  ManualEchoFdbkDelay, ManualEchoFdbkMix, ManualEchoFdbkAmt,
  ManualEchoReverbMix, ManualEchoReverbSize, ManualEchoReverbDamp,
  ManualLast = ManualEchoReverbDamp,
  Count };
std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
