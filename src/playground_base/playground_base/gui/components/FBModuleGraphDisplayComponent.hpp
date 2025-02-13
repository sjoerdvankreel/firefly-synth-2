#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBGraphRenderState;

class FBModuleGraphDisplayComponent final:
public juce::Component
{
public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphDisplayComponent);
  FBModuleGraphDisplayComponent(FBGraphRenderState* renderState);

  void paint(juce::Graphics& g) override;
  void RequestRerender(int moduleIndex);
  int TweakedModuleByUI() const { return _tweakedModuleByUI; }

private:
  int _tweakedModuleByUI = -1;
  FBModuleGraphRenderer _renderer = {};
  FBModuleGraphComponentData _data = {};  
  
  bool PrepareForRender(int moduleIndex);
  void PaintMarker(juce::Graphics& g, std::vector<float> const& points, int maxPoints, int marker);
  juce::Point<float> PointLocation(std::vector<float> const& points, int maxPoints, int point) const;
  void PaintSeries(juce::Graphics& g, juce::Colour color, std::vector<float> const& points, int maxPoints);
};