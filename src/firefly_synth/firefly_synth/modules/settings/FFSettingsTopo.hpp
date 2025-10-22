#pragma once

#include <memory>

struct FBStaticModule;
enum class FFSettingsVisualsMode { Basic, Always, IfFocus };
enum class FFSettingsGUIParam {
  UserScale, GraphVisualsMode, KnobVisualsMode,
  OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, EchoSelectedTab, Count };

std::unique_ptr<FBStaticModule> FFMakeSettingsTopo();