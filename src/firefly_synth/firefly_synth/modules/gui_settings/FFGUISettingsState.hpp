#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFGUISettingsGUIState final
{
  friend struct FFGUIState;
  friend std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();

  std::array<double, 1> userScale = {};
  std::array<double, 1> hilightMod = {};
  std::array<double, 1> hilightTweakMode = {};
  std::array<double, 1> fxSelectedTab = {};
  std::array<double, 1> oscSelectedTab = {};
  std::array<double, 1> mixSelectedTab = {};
  std::array<double, 1> envSelectedTab = {};
  std::array<double, 1> lfoSelectedTab = {};
  std::array<double, 1> echoSelectedTab = {};
  std::array<double, 1> knobVisualsMode = {};
  std::array<double, 1> graphVisualsMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUISettingsGUIState);
};
