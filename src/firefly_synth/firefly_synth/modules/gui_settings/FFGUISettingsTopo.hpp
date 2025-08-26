#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsRenderMode { Basic, Always, IfFocus };
enum class FFGUISettingsGUIParam {
  UserScale, ShowMatrix,
  GraphRenderMode, KnobRenderMode,
  OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, EchoSelectedTab, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();