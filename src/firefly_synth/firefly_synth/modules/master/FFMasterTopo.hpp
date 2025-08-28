#pragma once

#include <memory>

inline int constexpr FFMasterAuxCount = 4;

struct FBStaticModule;

enum class FFMasterTuningMode { Off, Note, Continuous };
enum class FFMasterCVOutput { Aux, ModWheel, PitchBend, Count };
enum class FFMasterPitchBendTarget { Off, Voice, Osc1, Osc2, Osc3, Osc4 };
enum class FFMasterParam { Aux, TuningMode, HostSmoothTime, ModWheel, PitchBend, PitchBendRange, PitchBendTarget, Count };

std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
