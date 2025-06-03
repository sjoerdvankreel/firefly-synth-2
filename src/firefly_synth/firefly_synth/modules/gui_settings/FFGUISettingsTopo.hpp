#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsGraphMode { Basic, Always, IfFocus };
enum class FFGUISettingsGUIParam { UserScale, GraphMode, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();