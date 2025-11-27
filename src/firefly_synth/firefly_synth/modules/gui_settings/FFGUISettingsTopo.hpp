#pragma once

#include <memory>

struct FBStaticModule;

enum class FFGUISettingsVisualsFromEngineMode { Off, On, IfFocus };
enum class FFGUISettingsGUIParam {
  UserScale, 
  HilightMod, HilightTweakMode,
  GraphVisualsMode, KnobVisualsMode,
  OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, EchoSelectedTab, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();