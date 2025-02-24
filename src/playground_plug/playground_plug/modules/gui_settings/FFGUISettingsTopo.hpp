#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsGUIParam { UserScale, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();