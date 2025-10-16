#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsGUIParam { ShowMatrix, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();