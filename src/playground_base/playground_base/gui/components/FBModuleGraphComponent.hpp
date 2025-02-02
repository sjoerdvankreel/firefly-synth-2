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

  void RequestRerender(int paramIndex);
  void paint(juce::Graphics& g) override;
  int TweakedParamByUI() const { return _tweakedParamByUI; }
  int _tweakedParamByUI = -1;

private:
  FBModuleGraphRenderer _renderer = {};
  FBModuleGraphComponentData _data = {};  
  
  bool PrepareForRender(int paramIndex);
  juce::Point<float> PointLocation(std::vector<float> const& points, int point) const;
  void PaintMarker(juce::Graphics& g, std::vector<float> const& points, int marker);
  void PaintSeries(juce::Graphics& g, juce::Colour color, std::vector<float> const& points);
};