#pragma once

#include <memory>

struct FBStaticModule;

enum class FFGUISettingsVisualsFromEngineMode { Off, On, IfFocus };
enum class FFGUISettingsGUIParam {
  UserScale, HilightMod, HilightTweakMode,
  GraphVisualsMode, KnobVisualsMode, Count };
std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();