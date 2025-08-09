#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsRenderMode { Basic, Always, IfFocus };
enum class FFGUISettingsGUIParam {
  UserScale, GraphRenderMode, KnobRenderMode,
  OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();