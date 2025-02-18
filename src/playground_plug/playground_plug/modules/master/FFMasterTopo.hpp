#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeMasterTopo();

enum class FFMasterGUIParam { UserScale, Count }; // todo userscale to own module
enum class FFMasterParam { HostSmoothTime, Gain, Voices, Count };