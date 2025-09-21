#pragma once

#include <memory>

inline int constexpr FFMasterAuxCount = 4;
inline int constexpr FFMasterUniMaxCount = 8;

struct FBStaticModule;

enum class FFMasterUniType { Off, Basic, Full };
enum class FFMasterCVOutput { Aux, ModWheel, PitchBend, Count };
enum class FFMasterPitchBendTarget { Off, Voice, Osc1, Osc2, Osc3, Osc4 };
enum class FFMasterTuningMode { Off, NoteOnce, NoteSlide, ModOnce, ModSlide };

enum class FFMasterParam { 
  Aux, TuningMode, HostSmoothTime, 
  ModWheel, PitchBend, PitchBendRange, PitchBendTarget, 
  UniType, UniCount, 
  UniFullOscGain, UniFullFirst = UniFullOscGain, UniFullOscPan, UniFullOscCoarse, UniFullOscFine, UniFullOscPhaseOffset, UniFullOscPhaseRand,
  UniFullLFORate, UniFullLFOMin, UniFullLFOMax, UniFullLFOSkewAX, UniFullLFOSkewAY, UniFullLFOPhaseOffset, UniFullLFOPhaseRand,
  UniFullEnvOffset, UniFullEnvStretch, UniFullVFXParamA, UniFullVFXParamB, UniFullVFXParamC, UniFullVFXParamD,
  UniFullEchoExtend, UniFullEchoFade,
  UniFullEchoTapLevel, UniFullEchoTapDelay, UniFullEchoTapBal,
  UniFullEchoFdbkDelay, UniFullEchoFdbkMix, UniFullEchoFdbkAmt,
  UniFullEchoReverbMix, UniFullEchoReverbSize, UniFullEchoReverbDamp, UniFullLast = UniFullEchoReverbDamp,
  Count };

std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
