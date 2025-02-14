#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class FBPlugGUI;
class FBParamControl;
class FBGridComponent;
class FBGraphRenderState;
class FBModuleGraphDisplayComponent;

struct FBModuleGraphComponentData;

class FBModuleGraphComponent final:
public juce::Component
{
  int _tweakedModuleByUI = -1;
  std::unique_ptr<FBParamControl> _graphControl = {};

  FBPlugGUI* const _plugGUI;
  juce::ToggleButton _controlActive;
  std::unique_ptr<FBGridComponent> _grid;
  std::unique_ptr<FBModuleGraphComponentData> _data;
  std::unique_ptr<FBModuleGraphDisplayComponent> _display;

  bool PrepareForRender(int moduleIndex);

public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  void RequestRerender(int moduleIndex);
  int TweakedModuleByUI() const { return _tweakedModuleByUI; }

  ~FBModuleGraphComponent();
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphComponent);
  FBModuleGraphComponent(FBPlugGUI* plugGUI, FBGraphRenderState* renderState);
};