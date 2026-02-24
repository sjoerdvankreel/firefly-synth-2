#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/graph/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/graph/FBModuleGraphDisplayComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

static float constexpr DefaultMarkerSize = 8.0f;
static float constexpr HalfDefaultMarkerSize = DefaultMarkerSize / 2.0f;
static float constexpr Padding = HalfDefaultMarkerSize;

FBModuleGraphDisplayComponent::
FBModuleGraphDisplayComponent(FBPlugGUI* plugGUI, FBModuleGraphComponentData const* data, int graphIndex):
Component(),
_plugGUI(plugGUI),
_data(data),
_graphIndex(graphIndex) {}

FBColorScheme const& 
FBModuleGraphDisplayComponent::FindColorSchemeFor(
  int moduleIndex, int moduleSlot) const
{
  auto fbLookAndFeel = FBGetLookAndFeelFor(_plugGUI);
  auto const& theme = fbLookAndFeel->Theme();
  if (theme.global.graphSchemeFollowsModule)
  {
    if (moduleIndex != -1 && moduleSlot != -1)
      if (auto gui = findParentComponentOfClass<FBPlugGUI>())
      {
        int rtModuleIndex = gui->HostContext()->Topo()->moduleTopoToRuntime.at({ moduleIndex, moduleSlot });
        auto moduleIter = theme.moduleColors.find(rtModuleIndex);
        if (moduleIter != theme.moduleColors.end())
          return theme.global.colorSchemes.at(moduleIter->second.colorScheme);
      }
  } 
  return fbLookAndFeel->FindColorSchemeFor(*this);
}  

Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  bool primarySeries, int secondaryIndex,
  std::vector<float> const& points,
  int point, bool stereo, bool left,
  int maxSizeAllSeries, float absMaxValueAllSeries) const
{
  point = std::clamp(point, 0, static_cast<int>(points.size()) - 1);
  float y = PointYLocation(primarySeries, secondaryIndex, points[point], stereo, left, absMaxValueAllSeries, true);
  float x = PointXLocation(primarySeries, secondaryIndex, static_cast<float>(point) / maxSizeAllSeries, true);
  return { x, y };
}

float
FBModuleGraphDisplayComponent::PointXLocation(
  bool primarySeries, int secondaryIndex,
  float pointRelative, bool withPadding) const
{
  FB_ASSERT(!std::isnan(pointRelative));
  auto const& points = _data->graphs[_graphIndex].GetPoints(primarySeries, secondaryIndex);
  if (points.plotLogarithmic)
  {
    float end = points.plotLogEnd;
    float start = points.plotLogStart;
    float expo = std::log(end / start) / std::log(2.0f);
    float realVal = start + (end - start) * pointRelative;
    pointRelative = std::log2(realVal / start) / expo;
  }
  if(withPadding)
    return Padding + pointRelative * (getWidth() - 2.0f * Padding);
  return pointRelative * getWidth();
}

float 
FBModuleGraphDisplayComponent::PointYLocation(
  bool primarySeries, int secondaryIndex,
  float pointYValue, bool stereo, 
  bool left, float absMaxValueAllSeries, bool withPadding) const
{
  FB_ASSERT(!std::isnan(pointYValue));
  float pointValue = pointYValue / absMaxValueAllSeries;
  if (_data->graphs[_graphIndex].GetPoints(primarySeries, secondaryIndex).bipolar)
    pointValue = FBToUnipolar(pointValue);
  if (stereo)
    pointValue = left ? 0.5f + pointValue * 0.5f: pointValue * 0.5f;
  if(withPadding)
    return Padding + (1.0f - pointValue) * (getHeight() - 2.0f * Padding);
  return (1.0f - pointValue) * getHeight();
}

void
FBModuleGraphDisplayComponent::PaintVerticalIndicator(
  Graphics& g, 
  bool primarySeries, int secondaryIndex, int point,
  int maxSizeAllSeries, float absMaxValueAllSeries)
{
  auto const& graphData = _data->graphs[_graphIndex];
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);

  float dashes[2] = { 4, 2 };
  g.setColour(scheme.text2.withAlpha(0.5f));
  if (!primarySeries)
    g.setColour(scheme.text2.withAlpha(0.25f));
  float x = PointXLocation(primarySeries, secondaryIndex, point / static_cast<float>(maxSizeAllSeries), true);
  float y0 = PointYLocation(primarySeries, secondaryIndex, 0.0f, false, false, absMaxValueAllSeries, true);
  float y1 = PointYLocation(primarySeries, secondaryIndex, absMaxValueAllSeries, false, false, absMaxValueAllSeries, true);
  g.drawDashedLine(Line<float>(x, y0, x, y1), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintMarker(
  Graphics& g, 
  bool primarySeries, int secondaryIndex,
  std::vector<float> const& points,
  int marker, bool isPointIndicator, bool stereo,
  bool left, int maxSizeAllSeries, float absMaxValueAllSeries)
{
  // It does happen every now and then.
  // Case in point: env length got 0 by modulation.
  if (points.size() == 0)
    return;

  auto const& graphData = _data->graphs[_graphIndex];
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);

  auto color = scheme.text2;
  if (_data->paintAsDisabled)
    color = color.darker(0.67f);
  if (!primarySeries)  
    color = color.withAlpha(0.5f);
  g.setColour(color);
  auto xy = PointLocation(primarySeries, secondaryIndex, points, marker, stereo, left, maxSizeAllSeries, absMaxValueAllSeries);

  float size = DefaultMarkerSize;
  bool fill = !isPointIndicator || _data->fillPointIndicators;
  if (isPointIndicator && _data->pointIndicatorSize != -1)
    size = (float)_data->pointIndicatorSize;

  float x = xy.getX() - size * 0.5f;
  float y = xy.getY() - size * 0.5f;

  if(fill)
    g.fillEllipse(x, y, size, size);
  else
    g.drawEllipse(x + size / 6.0f, y + size / 6.0f, size * 2.0f / 3.0f, size * 2.0f / 3.0f, 1.0f);
}

void
FBModuleGraphDisplayComponent::PaintSeries(
  juce::Graphics& g, 
  bool primarySeries, int secondaryIndex,
  std::vector<float> const& points,
  bool stereo, bool left,
  int maxSizeAllSeries, float absMaxValueAllSeries)
{
  if (points.empty())
    return;

  auto const& graphData = _data->graphs[_graphIndex];
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);

  Path fillPath;
  Path strokePath;
  float maxX = 0.0f;
  float maxY = 0.0f;
  float minY = std::numeric_limits<float>::max();
  float fillY = PointYLocation(primarySeries, secondaryIndex, 0.0f, stereo, left, absMaxValueAllSeries, true);
  maxY = std::max(maxY, fillY);
  minY = std::min(minY, fillY);
  auto strokeStart = PointLocation(primarySeries, secondaryIndex, points, 0, stereo, left, maxSizeAllSeries, absMaxValueAllSeries);
  strokePath.startNewSubPath(strokeStart);
  fillPath.startNewSubPath(PointXLocation(primarySeries, secondaryIndex, 0.0f, true), fillY);
  fillPath.lineTo(strokeStart);
  fillPath.lineTo(strokeStart); // for the rounding
  for (int i = 1; i < points.size(); i++)
  {
    auto thisPoint = PointLocation(primarySeries, secondaryIndex, points, i, stereo, left, maxSizeAllSeries, absMaxValueAllSeries);
    maxX = std::max(maxX, thisPoint.x);
    maxY = std::max(maxY, thisPoint.y);
    minY = std::min(minY, thisPoint.y);
    strokePath.lineTo(thisPoint);
    fillPath.lineTo(thisPoint);
  }
  fillPath.lineTo(maxX, PointYLocation(primarySeries, secondaryIndex, 0.0f, stereo, left, absMaxValueAllSeries, true));
  fillPath.closeSubPath();

  if (_data->graphs[_graphIndex].GetPoints(primarySeries, secondaryIndex).roundPathCorners)
  {
    fillPath = fillPath.createPathWithRoundedCorners(10.0f);
    strokePath = strokePath.createPathWithRoundedCorners(10.0f);
  }
  auto graphColor = scheme.primary;
  if (_data->paintAsDisabled)
    graphColor = graphColor.darker(0.67f);
  g.setColour(graphColor.withMultipliedAlpha(primarySeries? 0.4f: 0.1f));
  g.fillPath(fillPath);
  g.setColour(graphColor.withMultipliedAlpha(primarySeries? 1.0f: 0.4f));
  g.strokePath(strokePath, PathStrokeType(1.0f));
}

void
FBModuleGraphDisplayComponent::paint(Graphics& g)
{  
  int maxSizeAllSeries;
  float absMaxValueAllSeries;
  auto& graphData = _data->graphs[_graphIndex];
  auto const& primarySeries = graphData.primarySeries;
  auto const& secondarySeries = graphData.secondarySeries;
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);
  bool stereo = !primarySeries.r.empty();
  graphData.GetLimits(true, maxSizeAllSeries, absMaxValueAllSeries);
  absMaxValueAllSeries = std::max(1.0f, absMaxValueAllSeries);
  FB_ASSERT(graphData.secondarySeries.size() == 0 || _data->guiRenderType == FBGUIRenderType::Full);

  auto bounds = getLocalBounds();
  auto x0 = static_cast<int>(PointXLocation(true, -1, 0.0f, false));
  auto x1 = static_cast<int>(PointXLocation(true, -1, 1.0f, false));
  auto graphBounds = Rectangle<int>(x0, bounds.getY(), x1 - x0, bounds.getHeight());
  g.setColour(scheme.graphBackground.withAlpha(0.5f));
  g.fillRoundedRectangle(graphBounds.toFloat(), 5.0f);

  if (maxSizeAllSeries == 0)
  {
    auto lnf = FBGetLookAndFeelFor(_plugGUI);
    g.setFont(lnf->GetFont().withHeight(16.0f));
    g.setColour(Colours::white.withAlpha(0.25f));
    g.drawText("OFF", graphBounds, Justification::centred, false);
  }

  if (graphData.gridSizeY != -1)
  {
    g.setColour(Colours::white.withAlpha(0.25f));
    for (int r = 1; r < graphData.gridSizeY; r++)
    {
      float y = graphBounds.getHeight() / (float)graphData.gridSizeY * r;
      g.drawLine((float)graphBounds.getX(), y, graphBounds.getX() + (float)graphBounds.getWidth(), y, 1.0f);
    }        
  }

  if (maxSizeAllSeries != 0)
  {
    auto const& pvi = graphData.primarySeries.verticalIndicators;
    for (int i = 0; i < pvi.size(); i++)
    {
      FB_ASSERT(!stereo);
      PaintVerticalIndicator(g, 
        true, -1, pvi[i],
        maxSizeAllSeries, absMaxValueAllSeries);
    }
    for (int i = 0; i < graphData.secondarySeries.size(); i++)
    {
      auto const& svi = graphData.secondarySeries[i].points.verticalIndicators;
      for (int j = 0; j < svi.size(); j++)
        PaintVerticalIndicator(g, 
          false, i, svi[j],
          maxSizeAllSeries, absMaxValueAllSeries);
    }
  }

  if (maxSizeAllSeries != 0)
  {
    for (int i = 0; i < secondarySeries.size(); i++)
    {
      // if l is different, r is probably as well.
      bool equalsPrimarySeries = primarySeries.l.size() == secondarySeries[i].points.l.size();
      if(equalsPrimarySeries)
        for(int j = 0; j < primarySeries.l.size(); j++)
          if (std::abs(primarySeries.l[j] - secondarySeries[i].points.l[j]) >= 1e-5)
          {
            equalsPrimarySeries = false;
            break;
          }      

      auto const& points = secondarySeries[i].points;
      if (!equalsPrimarySeries)
      {
        PaintSeries(g,
          false, i, points.l,
          stereo, true, maxSizeAllSeries, absMaxValueAllSeries);
        if (stereo)
          PaintSeries(g,
            false, i, points.r,
            stereo, false, maxSizeAllSeries, absMaxValueAllSeries);
      }

      for (int j = 0; j < points.pointIndicators.size(); j++)
        PaintMarker(g, 
          false, i, points.l, points.pointIndicators[j],
          true, false, true, maxSizeAllSeries, absMaxValueAllSeries);

      int marker = secondarySeries[i].marker;
      if (marker != -1 && _data->drawMarkersSelector != nullptr && _data->drawMarkersSelector(_graphIndex))
      {
        FB_ASSERT(!stereo);
        PaintMarker(g, 
          false, i, points.l, marker,
          false, false, true, maxSizeAllSeries, absMaxValueAllSeries);
      }
    }

    PaintSeries(g, 
      true, -1, primarySeries.l,
      stereo, true, maxSizeAllSeries, absMaxValueAllSeries);
    if (stereo)
      PaintSeries(g, 
        true, -1, primarySeries.r,
        stereo, false, maxSizeAllSeries, absMaxValueAllSeries);

    for (int i = 0; i < primarySeries.pointIndicators.size(); i++)
      PaintMarker(g, 
        true, -1, primarySeries.l, primarySeries.pointIndicators[i],
        true, false, true, maxSizeAllSeries, absMaxValueAllSeries);

    FB_ASSERT(graphData.primaryMarkers.size() == 0 || _data->guiRenderType == FBGUIRenderType::Full);
    if (_data->drawMarkersSelector != nullptr && _data->drawMarkersSelector(_graphIndex))
      for (int i = 0; i < graphData.primaryMarkers.size(); i++)
      {
        FB_ASSERT(!stereo);
        PaintMarker(g, 
          true, -1, primarySeries.l, graphData.primaryMarkers[i],
          false, false, true, maxSizeAllSeries, absMaxValueAllSeries);
      }
  }
}