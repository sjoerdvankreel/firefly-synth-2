#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBGraphRenderState;

class FBModuleGraphComponent final:
public juce::Component
{
public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphComponent);
  FBModuleGraphComponent(FBGraphRenderState* renderState);

  void paint(juce::Graphics& g) override;
  void RequestRerender(int moduleIndex);
  int TweakedModuleByUI() const { return _tweakedModuleByUI; }

private:
  int _tweakedModuleByUI = -1;
  FBModuleGraphRenderer _renderer = {};
  FBModuleGraphComponentData _data = {};  
  
  bool PrepareForRender(int moduleIndex);
  juce::Point<float> PointLocation(std::vector<float> const& points, int point) const;
  void PaintMarker(juce::Graphics& g, std::vector<float> const& points, int marker);
  void PaintSeries(juce::Graphics& g, juce::Colour color, std::vector<float> const& points);
};