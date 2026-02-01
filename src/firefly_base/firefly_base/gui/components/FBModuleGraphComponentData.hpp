#pragma once

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <vector>
#include <string>
#include <functional>

typedef std::function<bool(int graphIndex)>
FBModuleGraphDrawMarkersSelector;

class FBGraphRenderState;

struct FBModuleGraphPoints final
{
  float plotLogEnd = {};
  float plotLogStart = {};
  bool bipolar = false;
  bool plotLogarithmic = false;
  bool roundPathCorners = false;
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
  int moduleSlot = -1;
  int moduleIndex = -1;
  bool anyExchangeActive = false;
  std::string title = {};
  std::string defaultSubText = {};
  std::string defaultMainText = {};
  std::string exchangeSubText = {};
  std::string exchangeMainText = {};
  std::vector<int> primaryMarkers = {};
  FBModuleGraphPoints primarySeries = {};
  std::vector<FBModuleGraphSecondarySeries> secondarySeries = {};

  void MergeStereo();
  void ScaleBy(float factor);
  void ScaleToSelfNormalized();
  bool GetScaleFactorToNormalized(float& factor) const;
  void GetLimits(bool includePrimary, int& maxSizeAllSeries, float& absMaxValueAllSeries) const;

  FBModuleGraphPoints& GetPoints(bool primary, int secondaryIndex) 
  { return primary ? primarySeries : secondarySeries[secondaryIndex].points; }
  FBModuleGraphPoints const& GetPoints(bool primary, int secondaryIndex) const 
  { return primary ? primarySeries : secondarySeries[secondaryIndex].points; }
};

struct FBModuleGraphComponentData final
{
  int pixelWidth = -1;
  int fixedGraphIndex = -1;
  int pointIndicatorSize = -1;
  bool paintAsDisabled = false;
  bool fillPointIndicators = false;
  bool skipDrawOnEqualsPrimary = true;
  FBGraphRenderState* renderState = {};
  std::vector<FBModuleGraphData> graphs = {};
  FBGUIRenderType guiRenderType = FBGUIRenderType::Basic;
  FBModuleGraphDrawMarkersSelector drawMarkersSelector = {};

  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleGraphComponentData);
};