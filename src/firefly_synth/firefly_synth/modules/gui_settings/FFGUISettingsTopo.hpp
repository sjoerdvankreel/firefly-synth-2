#pragma once

#include <memory>

struct FBStaticModule;
enum class FFGUISettingsVisualsMode { Basic, Always, IfFocus };

enum class FFGUISettingsParam {
  FlushAudioToggle, Count };
enum class FFGUISettingsGUIParam {
  UserScale, ShowMatrix, VisualsMode,
  OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, EchoSelectedTab, Count };

std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();