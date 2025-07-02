#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsGraphMode { Basic, Always, IfFocus };
enum class FFGUISettingsGUIParam {
  UserScale, GraphMode, OscSelectedTab, 
  FXSelectedTab, MixSelectedTab, EnvSelectedTab, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();