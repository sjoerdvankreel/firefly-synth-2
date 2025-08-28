#pragma once

#include <memory>

inline int constexpr FFMasterAuxCount = 4;

struct FBStaticModule;

enum class FFMasterCVOutput { Aux, Count };
enum class FFMasterTuningMode { Off, Note, Continuous };
enum class FFMasterParam { Aux, TuningMode, HostSmoothTime, Count };

std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
