#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_plug/modules/master/FFMasterState.hpp>

struct FFGUIState final
{
  FFMasterGUIState master = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGUIState);
};