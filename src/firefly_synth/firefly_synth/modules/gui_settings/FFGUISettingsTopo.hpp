#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsGraphMode { Basic, Always, IfFocus };
enum class FFGUISettingsGUIParam {
  UserScale, GraphMode, OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, MatrixSelectedTab, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();