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
  std::vector<int> pointIndicators = {};
  std::vector<int> verticalIndicators = {};
};

struct FBModuleGraphSecondarySeries final
{
  int marker = -1;
  FBModuleGraphPoints points = {};
};

struct FBModuleGraphData final
{
private:
  mutable int _maxSizeAllSeries = -1;
  mutable float _absMaxValueAllSeries = {};

public:
  std::string text = {};
  std::string moduleName = {};
  std::vector<int> primaryMarkers = {};
  FBModuleGraphPoints primarySeries = {};
  std::vector<FBModuleGraphSecondarySeries> secondarySeries = {};
  void GetLimits(int& maxSizeAllSeries, float& absMaxValueAllSeries) const;
};

struct FBModuleGraphComponentData final
{
  int pixelWidth = -1;
  bool bipolar = false;
  bool drawMarkers = false;
  bool drawClipBoundaries = false;
  bool skipDrawOnEqualsPrimary = true;
  FBGraphRenderState* renderState = {};
  std::vector<FBModuleGraphData> graphs = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleGraphComponentData);
};