#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class FBPlugGUI;
class FBGridComponent;
class FBGraphRenderState;
class FBModuleGraphDisplayComponent;

struct FBTopoIndices;
struct FBStaticModule;
struct FBStaticModuleGraph;
struct FBModuleGraphComponentData;

class FBModuleGraphComponent final:
public juce::Component
{
  int _tweakedModuleByUI = -1;
  std::unique_ptr<juce::Slider> _graphControl = {};

  FBPlugGUI* const _plugGUI;
  juce::ToggleButton _controlActive;
  std::unique_ptr<FBGridComponent> _grid;
  std::unique_ptr<FBModuleGraphComponentData> _data;
  std::unique_ptr<FBModuleGraphDisplayComponent> _display;

  void RecreateGraphControl();
  bool PrepareForRender(int moduleIndex);
  FBTopoIndices const& TopoIndicesFor(int moduleIndex) const;
  FBStaticModule const& StaticModuleFor(int moduleIndex) const;
  std::unique_ptr<juce::Slider> MakeGraphControl(FBStaticModuleGraph const& topo) const;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  void RequestRerender(int moduleIndex);
  int TweakedModuleByUI() const { return _tweakedModuleByUI; }

  ~FBModuleGraphComponent();
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphComponent);
  FBModuleGraphComponent(FBPlugGUI* plugGUI, FBGraphRenderState* renderState);
};