#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <string>
#include <optional>

class FBGraphRenderState;

struct FBModuleGraphSecondarySeries final
{
  int marker = -1;
  std::vector<float> points = {};
};

struct FBModuleGraphComponentData final
{
  int pixelWidth = -1;
  bool bipolar = false;
  std::string text = {};
  std::string moduleName = {};
  FBGraphRenderState* renderState = {};
  std::vector<int> primaryMarkers = {};
  std::vector<float> primarySeries = {};
  std::vector<FBModuleGraphSecondarySeries> secondarySeries = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleGraphComponentData);
};