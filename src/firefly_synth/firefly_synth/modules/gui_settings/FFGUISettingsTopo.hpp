#pragma once

#include <memory>

struct FBStaticModule;

// TODO better names
enum class FFGUISettingsVisualsMode { Basic, Always, IfFocus };
enum class FFGUISettingsGUIParam {
  UserScale, 
  HilightMod, HilightTweak,
  GraphVisualsMode, KnobVisualsMode,
  OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, EchoSelectedTab, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();