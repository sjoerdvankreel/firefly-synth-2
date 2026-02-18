#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <chrono>
#include <memory>
#include <functional>

class FBPlugGUI;
class FBGridComponent;
class FBCardComponent;
class FBMarginComponent;
class FBGraphRenderState;
class FBModuleGraphTitleComponent;
class FBModuleGraphDisplayComponent;

struct FBTopoIndices;
struct FBStaticModule;
struct FBStaticModuleGraph;
struct FBModuleGraphComponentData;

class FBModuleGraphComponent final:
public juce::Component
{
  FBPlugGUI* const _plugGUI;
  bool const _detailGraphs;

  int _graphCount = -1;
  int _tweakedModuleByUI = -1;
  FBModuleGraphLayout _layout = {};
  std::unique_ptr<FBGridComponent> _grid = {};
  std::function<FBGUIRenderType()> _getCurrentRenderType;

  int const _fixedToRuntimeModuleIndex;
  int const _fixedToGraphIndex;
  std::unique_ptr<FBModuleGraphComponentData> _data;
  std::vector<std::unique_ptr<FBCardComponent>> _cards = {};
  std::vector<std::unique_ptr<FBModuleGraphTitleComponent>> _titles = {};
  std::vector<std::unique_ptr<FBModuleGraphDisplayComponent>> _displays = {};
  std::vector<std::unique_ptr<FBGridComponent>> _displayAndTitleGrids = {};
  std::chrono::high_resolution_clock::time_point _updated = {};

  bool PrepareForRender(int moduleIndex);
  FBTopoIndices const& TopoIndicesFor(int moduleIndex) const;
  FBStaticModule const& StaticModuleFor(int moduleIndex) const;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  void RequestRerender(int moduleIndex);

  int TweakedModuleByUI() const { return _tweakedModuleByUI; }
  int FixedToGraphIndex() const { return _fixedToGraphIndex; }
  int FixedToRuntimeModuleIndex() const { return _fixedToRuntimeModuleIndex; }

  ~FBModuleGraphComponent();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBModuleGraphComponent);
  FBModuleGraphComponent(
    FBPlugGUI* plugGUI,
    bool detailGraphs,
    FBGraphRenderState* renderState, 
    int fixedToRuntimeModuleIndex, 
    int fixedToGraphIndex, 
    std::function<FBGUIRenderType()> getCurrentRenderType);
};