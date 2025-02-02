#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <string>
#include <optional>

class FBGraphRenderState;

struct FBModuleGraphSecondaryData final
{
  int marker = -1;
  std::vector<float> points = {};
};

struct FBModuleGraphComponentData final
{
  std::string text = {};
  FBGraphRenderState* state = {};
  std::vector<float> primaryPoints = {};
  std::vector<FBModuleGraphSecondaryData> secondaryData = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleGraphComponentData);
};