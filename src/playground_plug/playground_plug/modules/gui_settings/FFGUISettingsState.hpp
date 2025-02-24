#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFGUISettingsGUIState final
{
  friend struct FFGUIState;
  std::array<float, 1> userScale = {};
  friend std::unique_ptr<FBStaticModule> FFMakeGUISettingsTopo();
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUISettingsGUIState);
};
