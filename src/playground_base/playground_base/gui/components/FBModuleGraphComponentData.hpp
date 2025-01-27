#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <string>

struct FBModuleGraphComponentData final
{
  std::string text = {};
  std::vector<float> points = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleGraphComponentData);
};