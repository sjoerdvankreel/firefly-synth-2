#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <string>
#include <optional>

class FBGraphRenderState;

struct FBModuleGraphPoints final
{
  std::vector<float> l = {};
  std::vector<float> r = {};
};

struct FBModuleGraphSecondarySeries final
{
  int marker = -1;
  FBModuleGraphPoints points = {};
};

struct FBModuleGraphComponentData final
{
  int pixelWidth = -1;
  bool bipolar = false;
  bool drawMarkers = false;
  bool drawClipBoundaries = false;
  bool skipDrawOnEqualsPrimary = true;
  std::string text = {};
  std::string moduleName = {};
  FBGraphRenderState* renderState = {};
  std::vector<int> primaryMarkers = {};
  FBModuleGraphPoints primarySeries = {};
  std::vector<FBModuleGraphSecondarySeries> secondarySeries = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleGraphComponentData);
};