#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <memory>

struct FBStaticModule;

class FFGUISettingsGUIState final
{
  friend struct FFGUIState;
  std::array<double, 1> userScale = {};
  std::array<double, 1> graphMode = {};
  friend std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUISettingsGUIState);
};
