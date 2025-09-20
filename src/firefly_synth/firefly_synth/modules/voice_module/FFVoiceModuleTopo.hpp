#pragma once

#include <memory>

struct FBStaticModule;
inline int constexpr FFVoiceModuleUniMaxCount = 8;

enum class FFVoiceModulePortaType { Off, On, Auto };
enum class FFVoiceModulePortaMode { Always, Section };

enum class FFVoiceModuleParam { 
  Coarse, Env5ToCoarse, Fine, LFO5ToFine, 
  PortaType, PortaMode, PortaSync, PortaTime, PortaBars, 
  PortaSectionAmpAttack, PortaSectionAmpRelease, 
  UniCount, UniEnvOffset, UniEnvStretch, UniVFXParamA, UniVFXParamB, UniVFXParamC, UniVFXParamD,
  UniOscCoarse, UniOscFine, UniOscGain, UniOscPan, UniOscPhaseOffset, UniOscPhaseRand,
  UniLFORate, UniLFOPhaseOffset, UniLFOPhaseOffsetRand, UniLFOMin, UniLFOMax, UniLFOSkewAX, UniLFOSkewAY,
  Count };
std::unique_ptr<FBStaticModule> FFMakeVoiceModuleTopo();