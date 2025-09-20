#pragma once

#include <memory>

inline int constexpr FFMasterAuxCount = 4;
inline int constexpr FFMasterUniMaxCount = 8;

struct FBStaticModule;

enum class FFMasterCVOutput { Aux, ModWheel, PitchBend, Count };
enum class FFMasterPitchBendTarget { Off, Voice, Osc1, Osc2, Osc3, Osc4 };
enum class FFMasterTuningMode { Off, NoteOnce, NoteSlide, ModOnce, ModSlide };

enum class FFMasterParam { 
  Aux, TuningMode, HostSmoothTime, 
  ModWheel, PitchBend, PitchBendRange, PitchBendTarget, 
  UniCount, UniEnvOffset, UniEnvStretch, UniVFXParamA, UniVFXParamB, UniVFXParamC, UniVFXParamD,
  UniOscCoarse, UniOscFine, UniOscGain, UniOscPan, UniOscPhaseOffset, UniOscPhaseRand,
  UniLFORate, UniLFOPhaseOffset, UniLFOPhaseRand, UniLFOMin, UniLFOMax, UniLFOSkewAX, UniLFOSkewAY,
  Count };

std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
