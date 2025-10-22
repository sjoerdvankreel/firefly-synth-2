#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <memory>

struct FBStaticModule;

class FFSettingsGUIState final
{
  friend struct FFGUIState;
  std::array<double, 1> userScale = {};
  std::array<double, 1> fxSelectedTab = {};
  std::array<double, 1> oscSelectedTab = {};
  std::array<double, 1> mixSelectedTab = {};
  std::array<double, 1> envSelectedTab = {};
  std::array<double, 1> lfoSelectedTab = {};
  std::array<double, 1> echoSelectedTab = {};
  std::array<double, 1> knobVisualsMode = {};
  std::array<double, 1> graphVisualsMode = {};
  friend std::unique_ptr<FBStaticModule> FFMakeSettingsTopo();
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFSettingsGUIState);
};
