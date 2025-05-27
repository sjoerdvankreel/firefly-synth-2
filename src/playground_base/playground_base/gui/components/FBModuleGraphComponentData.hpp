#pragma once

#include <playground_base/base/shared/FBUtility.hpp>

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

struct FBModuleGraphSeriesData final
{
  std::string text = {};
  std::string moduleName = {};
  std::vector<int> primaryMarkers = {};
  FBModuleGraphPoints primarySeries = {};
  std::vector<int> verticalIndicators1 = {};
  std::vector<int> verticalIndicators2 = {};
  std::vector<FBModuleGraphSecondarySeries> secondarySeries = {};
};

struct FBModuleGraphComponentData final
{
  int pixelWidth = -1;
  bool bipolar = false;
  bool drawMarkers = false;
  bool drawClipBoundaries = false;
  bool skipDrawOnEqualsPrimary = true;
  FBGraphRenderState* renderState = {};
  std::vector<FBModuleGraphSeriesData> series = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleGraphComponentData);
};