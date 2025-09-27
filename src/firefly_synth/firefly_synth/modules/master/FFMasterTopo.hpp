#pragma once

#include <memory>

struct FBStaticModule;
inline int constexpr FFMasterAuxCount = 4;

enum class FFMasterCVOutput { Aux, ModWheel, PitchBend, Count };
enum class FFMasterPitchBendTarget { Off, Voice, Osc1, Osc2, Osc3, Osc4 };

enum class FFMasterParam { 
  Aux, HostSmoothTime, ModWheel, 
  PitchBend, PitchBendRange, PitchBendTarget, Count };
std::unique_ptr<FBStaticModule> FFMakeMasterTopo();