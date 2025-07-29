#pragma once

#include <memory>

inline int constexpr FFMasterAuxCount = 6;

struct FBStaticModule;
enum class FFMasterCVOutput { Aux, Count };
enum class FFMasterParam { HostSmoothTime, Aux, Count };
std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
