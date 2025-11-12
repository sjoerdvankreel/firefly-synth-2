#pragma once

#include <memory>

struct FBStaticModule;
inline int constexpr FFMasterAuxCount = 4;

enum class FFMasterCVOutput { 
  Aux, ModWheel, PitchBend, PitchBendRaw, 
  LastKeyPitch, LowKeyPitch, HighKeyPitch, LowVeloPitch, HighVeloPitch,
  LastKeyPitchSmth, LowKeyPitchSmth, HighKeyPitchSmth, LowVeloPitchSmth, HighVeloPitchSmth,
  Count
};
enum class FFMasterPitchBendTarget { Off, Global, Osc1, Osc2, Osc3, Osc4 };

enum class FFMasterParam { 
  Aux, ModWheel, PitchBend, PitchBendRange, PitchBendTarget, Count };
std::unique_ptr<FBStaticModule> FFMakeMasterTopo();