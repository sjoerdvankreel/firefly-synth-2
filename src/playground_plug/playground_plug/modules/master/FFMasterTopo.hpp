#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeMasterTopo();

enum class FFMasterParam { Smooth, Gain, Count };