#pragma once

#include <memory>

struct FBStaticModule;
enum class FFSettingsVisualsMode { Basic, Always, IfFocus };
enum class FFSettingsParam {
  HostSmoothTime, ReceiveNotes, Count };
enum class FFSettingsGUIParam {
  UserScale, 
  HilightMod, HilightTweak,
  GraphVisualsMode, KnobVisualsMode,
  OscSelectedTab, FXSelectedTab, LFOSelectedTab, 
  MixSelectedTab, EnvSelectedTab, EchoSelectedTab, Count };

std::unique_ptr<FBStaticModule> FFMakeSettingsTopo(bool isFx);