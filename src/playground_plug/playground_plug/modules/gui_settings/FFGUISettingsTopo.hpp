#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsGUIParam { GraphTrack, UserScale, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();