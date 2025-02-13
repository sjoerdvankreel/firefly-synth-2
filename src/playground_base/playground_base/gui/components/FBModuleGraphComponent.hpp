#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class FBGraphRenderState;
class FBModuleGraphDisplayComponent;
struct FBModuleGraphComponentData;

class FBModuleGraphComponent final:
public juce::Component
{
  int _tweakedModuleByUI = -1;
  std::unique_ptr<FBModuleGraphComponentData> _data;
  std::unique_ptr<FBModuleGraphDisplayComponent> _display;
  bool PrepareForRender(int moduleIndex);

public:
  ~FBModuleGraphComponent();
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphComponent);
  FBModuleGraphComponent(FBGraphRenderState* renderState);

  void resized() override;
  void paint(juce::Graphics& g) override;
  void RequestRerender(int moduleIndex);
  int TweakedModuleByUI() const { return _tweakedModuleByUI; }
};