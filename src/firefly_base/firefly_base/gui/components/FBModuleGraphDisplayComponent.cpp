#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/components/FBModuleGraphDisplayComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

static float constexpr Padding = 3.0f;
static float constexpr DefaultMarkerSize = 8.0f;
static float constexpr HalfDefaultMarkerSize = 4.0f;

FBModuleGraphDisplayComponent::
FBModuleGraphDisplayComponent(FBModuleGraphComponentData const* data, bool withBorder):
Component(),
_withBorder(withBorder),
_data(data) {}

FBColorScheme const& 
FBModuleGraphDisplayComponent::FindColorSchemeFor(
  int moduleIndex, int moduleSlot) const
{
  auto const& theme = FBGetLookAndFeel()->Theme();
  if (theme.graphSchemeFollowsModule)
  {
    if (moduleIndex != -1 && moduleSlot != -1)
      if (auto gui = findParentComponentOfClass<FBPlugGUI>())
      {
        int rtModuleIndex = gui->HostContext()->Topo()->moduleTopoToRuntime.at({ moduleIndex, moduleSlot });
        auto moduleIter = theme.moduleColors.find(rtModuleIndex);
        if (moduleIter != theme.moduleColors.end())
          return theme.colorSchemes.at(moduleIter->second.colorScheme);
      }
  } 

  if (auto gui = findParentComponentOfClass<FBPlugGUI>())
  {
    juce::Component const* cp = this;
    while (cp != nullptr)
    {
      if (auto tc = dynamic_cast<IFBThemingComponent const*>(cp))
        if (auto s = tc->GetScheme(theme))
          return *s;
      cp = cp->getParentComponent();
    }
  }

  return theme.defaultColorScheme;
}

Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  int graph, std::vector<float> const& points,
  int point, bool stereo, bool left,
  int maxSizeAllSeries, float absMaxValueAllSeries) const
{
  point = std::clamp(point, 0, static_cast<int>(points.size()) - 1);
  float y = PointYLocation(graph, points[point], stereo, left, absMaxValueAllSeries, true);
  float x = PointXLocation(graph, static_cast<float>(point) / maxSizeAllSeries, true);
  return { x, y };
}

float
FBModuleGraphDisplayComponent::PointXLocation(
  int graph, float pointRelative, bool withPadding) const
{
  FB_ASSERT(!std::isnan(pointRelative));
  float graphCount = static_cast<float>(_data->graphs.size());
  float gapWidth = 3.0f;
  float gapCount = graphCount - 1.0f;
  float graphWidth = std::floor((getWidth() - gapCount * gapWidth) / graphCount);
  float graphLeft = graph * (graphWidth + gapWidth);
  if (graph == graphCount - 1)
    graphWidth += (getWidth() - gapCount * gapWidth - graphCount * graphWidth);
  if(withPadding)
    return graphLeft + Padding + pointRelative * (graphWidth - 2.0f * Padding);
  return graphLeft + pointRelative * graphWidth;
}

float 
FBModuleGraphDisplayComponent::PointYLocation(
  int graph, float pointYValue, bool stereo, 
  bool left, float absMaxValueAllSeries, bool withPadding) const
{
  FB_ASSERT(!std::isnan(pointYValue));
  float pointValue = pointYValue / absMaxValueAllSeries;
  if (_data->graphs[graph].bipolar)
    pointValue = FBToUnipolar(pointValue);
  if (stereo)
    pointValue = left ? 0.5f + pointValue * 0.5f: pointValue * 0.5f;
  if(withPadding)
    return HalfDefaultMarkerSize + Padding + (1.0f - pointValue) * (getHeight() - DefaultMarkerSize - 2.0f * Padding);
  return HalfDefaultMarkerSize + (1.0f - pointValue) * (getHeight() - DefaultMarkerSize);
}

void
FBModuleGraphDisplayComponent::PaintVerticalIndicator(
  Graphics& g, int graph, int point, bool primary,
  int maxSizeAllSeries, float absMaxValueAllSeries)
{
  auto const& graphData = _data->graphs[graph];
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);

  float dashes[2] = { 4, 2 };
  g.setColour(scheme.text.withAlpha(0.5f));
  if (!primary)
    g.setColour(scheme.text.withAlpha(0.25f));
  float x = PointXLocation(graph, point / static_cast<float>(maxSizeAllSeries), true);
  float y0 = PointYLocation(graph, 0.0f, false, false, absMaxValueAllSeries, true);
  float y1 = PointYLocation(graph, absMaxValueAllSeries, false, false, absMaxValueAllSeries, true);
  g.drawDashedLine(Line<float>(x, y0, x, y1), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintMarker(
  Graphics& g, 
  int graph, std::vector<float> const& points,
  int marker, bool primary, bool isPointIndicator, bool stereo,
  bool left, int maxSizeAllSeries, float absMaxValueAllSeries)
{
  // It does happen every now and then.
  // Case in point: env length got 0 by modulation.
  if (points.size() == 0)
    return;

  auto const& graphData = _data->graphs[graph];
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);

  auto color = scheme.text;
  if (_data->paintAsDisabled)
    color = color.darker(0.67f);
  if (!primary)
    color = color.withAlpha(0.5f);
  g.setColour(color);
  auto xy = PointLocation(graph, points, marker, stereo, left, maxSizeAllSeries, absMaxValueAllSeries);

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
FBModuleGraphDisplayComponent::PaintClipBoundaries(
  juce::Graphics& g,
  int graph, bool stereo, bool left,
  float absMaxValueAllSeries)
{
  auto const& graphData = _data->graphs[graph];
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);

  float dashes[2] = { 4.0, 2.0 };
  float x0 = PointXLocation(graph, 0.0f, true);
  float x1 = PointXLocation(graph, 1.0f, true);
  float upperY = PointYLocation(graph, 1.0f, stereo, left, absMaxValueAllSeries, true);
  float lowerY = PointYLocation(graph, _data->graphs[graph].bipolar? -1.0f: 0.0f, stereo, left, absMaxValueAllSeries, true);
  g.setColour(scheme.text.withAlpha(0.25f));
  g.drawDashedLine(Line<float>(x0, upperY, x1, upperY), dashes, 2);
  g.drawDashedLine(Line<float>(x0, lowerY, x1, lowerY), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintSeries(
  juce::Graphics& g, bool primary,
  int graph, std::vector<float> const& points,
  bool stereo, bool left,
  int maxSizeAllSeries, float absMaxValueAllSeries)
{
  if (points.empty())
    return;

  auto const& graphData = _data->graphs[graph];
  auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);
  auto color = primary ? scheme.primary : scheme.paramSecondary;

  Path fillPath;
  Path strokePath;
  fillPath.startNewSubPath(PointXLocation(graph, 0.0f, true), PointYLocation(graph, 0.0f, stereo, left, absMaxValueAllSeries, true));
  strokePath.startNewSubPath(PointLocation(graph, points, 0, stereo, left, maxSizeAllSeries, absMaxValueAllSeries));
  for (int i = 1; i < points.size(); i++)
  {
    fillPath.lineTo(PointLocation(graph, points, i, stereo, left, maxSizeAllSeries, absMaxValueAllSeries));
    strokePath.lineTo(PointLocation(graph, points, i, stereo, left, maxSizeAllSeries, absMaxValueAllSeries));
  }
  fillPath.lineTo(PointXLocation(graph, 1.0f, true), PointYLocation(graph, 0.0f, stereo, left, absMaxValueAllSeries, true));
  fillPath.closeSubPath();
  if (_data->paintAsDisabled)
    color = color.darker(0.67f);
  if (primary)
  {
    g.setColour(color.withAlpha(0.33f));
    g.fillPath(fillPath);
  }
  g.setColour(color);
  g.strokePath(strokePath, PathStrokeType(1.0f));
}

void
FBModuleGraphDisplayComponent::paint(Graphics& g)
{  
  for (int graph = 0; graph < _data->graphs.size(); graph++)
  {
    int maxSizeAllSeries;
    float absMaxValueAllSeries;
    auto& graphData = _data->graphs[graph];
    auto const& primarySeries = graphData.primarySeries;
    auto const& secondarySeries = graphData.secondarySeries;
    auto const& scheme = FindColorSchemeFor(graphData.moduleIndex, graphData.moduleSlot);
    bool stereo = !primarySeries.r.empty();
    graphData.GetLimits(maxSizeAllSeries, absMaxValueAllSeries);
    FB_ASSERT(graphData.secondarySeries.size() == 0 || _data->guiRenderType == FBGUIRenderType::Full);

    auto bounds = getLocalBounds();
    auto x0 = static_cast<int>(PointXLocation(graph, 0.0f, false));
    auto x1 = static_cast<int>(PointXLocation(graph, 1.0f, false));
    auto graphBounds = Rectangle<int>(x0, bounds.getY(), x1 - x0, bounds.getHeight());
    g.setColour(scheme.graphBackground);
    g.fillRoundedRectangle(graphBounds.toFloat(), 5.0f);

    if (_withBorder)
    {
      g.setColour(scheme.sectionBorder.withAlpha(0.125f));
      g.drawRoundedRectangle(graphBounds.toFloat(), 5.0f, 2.0f);
    }

    if (maxSizeAllSeries != 0)
    {
      auto const& pvi = graphData.primarySeries.verticalIndicators;
      for (int i = 0; i < pvi.size(); i++)
      {
        FB_ASSERT(!stereo);
        PaintVerticalIndicator(g, graph, pvi[i],
          true, maxSizeAllSeries, absMaxValueAllSeries);
      }
      for (int i = 0; i < graphData.secondarySeries.size(); i++)
      {
        auto const& svi = graphData.secondarySeries[i].points.verticalIndicators;
        for (int j = 0; j < svi.size(); j++)
          PaintVerticalIndicator(g, graph, svi[j],
            false, maxSizeAllSeries, absMaxValueAllSeries);
      }
    }

    if (graphData.subtext.size())
    {
      g.setColour(scheme.text.withAlpha(0.33f));
      g.setFont(FBGUIGetFont().withHeight(20.0f));
      g.drawText(graphData.subtext, graphBounds, Justification::centred, false);
    }

    if (maxSizeAllSeries != 0)
    {
      for (int i = 0; i < secondarySeries.size(); i++)
      {
        int marker = secondarySeries[i].marker;
        auto const& points = secondarySeries[i].points;
        PaintSeries(g, false, graph, points.l,
          stereo, true, maxSizeAllSeries, absMaxValueAllSeries);
        if (stereo)
          PaintSeries(g, false, graph, points.r,
            stereo, false, maxSizeAllSeries, absMaxValueAllSeries);

        for (int j = 0; j < points.pointIndicators.size(); j++)
          PaintMarker(g, graph, points.l, points.pointIndicators[j],
            false, true, false, true, maxSizeAllSeries, absMaxValueAllSeries);

        if (marker != -1 && _data->drawMarkersSelector != nullptr && _data->drawMarkersSelector(graph))
        {
          FB_ASSERT(!stereo);
          PaintMarker(g, graph, points.l, marker,
            false, false, false, true, maxSizeAllSeries, absMaxValueAllSeries);
        }
      }

      PaintSeries(g, true, graph, primarySeries.l,
        stereo, true, maxSizeAllSeries, absMaxValueAllSeries);
      if (stereo)
        PaintSeries(g, true, graph, primarySeries.r,
          stereo, false, maxSizeAllSeries, absMaxValueAllSeries);

      for (int i = 0; i < primarySeries.pointIndicators.size(); i++)
        PaintMarker(g, graph, primarySeries.l, primarySeries.pointIndicators[i],
          true, true, false, true, maxSizeAllSeries, absMaxValueAllSeries);

      FB_ASSERT(graphData.primaryMarkers.size() == 0 || _data->guiRenderType == FBGUIRenderType::Full);
      if (_data->drawMarkersSelector != nullptr && _data->drawMarkersSelector(graph))
        for (int i = 0; i < graphData.primaryMarkers.size(); i++)
        {
          FB_ASSERT(!stereo);
          PaintMarker(g, graph, primarySeries.l, graphData.primaryMarkers[i],
            true, false, false, true, maxSizeAllSeries, absMaxValueAllSeries);
        }

      if (graphData.drawClipBoundaries)
      {
        PaintClipBoundaries(g, graph, stereo, false, absMaxValueAllSeries);
        if (stereo)
          PaintClipBoundaries(g, graph, stereo, true, absMaxValueAllSeries);
      }
    }

    if (graphData.title.size())
    {
      float const labelPad = 4.0f;
      auto newFont = FBGUIGetFont().withHeight(14.0f);
      auto textSize = TextLayout::getStringBounds(newFont, graphData.title);
      auto textBounds = Rectangle<float>(
        graphBounds.getX() + graphBounds.getWidth() - textSize.getWidth() - 2.0f * labelPad,
        (float)graphBounds.getY() + labelPad,
        textSize.getWidth() + labelPad,
        textSize.getHeight() + labelPad);
      g.setColour(scheme.sectionBackground.withAlpha(0.75f));
      g.fillRoundedRectangle(textBounds, 2.0f);
      g.setColour(scheme.text);
      g.setFont(newFont);
      g.drawText(graphData.title, textBounds, Justification::centred, false);
    }
  }
}