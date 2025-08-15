#pragma once

#include <memory>

struct FBStaticModule;
enum class FFExternalParam { HostSmoothTime, Count };
std::unique_ptr<FBStaticModule> FFMakeExternalTopo();
