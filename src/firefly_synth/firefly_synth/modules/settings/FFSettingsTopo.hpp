#pragma once

#include <memory>

struct FBStaticModule;
enum class FFSettingsParam { HostSmoothTime, ReceiveNotes, Count };
std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool isFx);