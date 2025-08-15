#pragma once

#include <memory>

inline int constexpr FFMasterAuxCount = 4;

struct FBStaticModule;
enum class FFMasterParam { Aux, Count };
enum class FFMasterCVOutput { Aux, Count };
std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
