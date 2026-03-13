#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>

#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/graph/FBModuleGraphComponent.hpp>
#include <firefly_base/gui/graph/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/graph/FBModuleGraphTextComponent.hpp>
#include <firefly_base/gui/graph/FBModuleGraphDisplayComponent.hpp>

using namespace juce;

FBModuleGraphComponent::
~FBModuleGraphComponent() {}

FBModuleGraphComponent::
FBModuleGraphComponent(
  FBPlugGUI* plugGUI,
  bool detailGraphs,
  FBGraphRenderState* renderState,
  int fixedToRuntimeModuleIndex,
  int fixedToGraphIndex,
  std::function<FBGUIRenderType()> getCurrentRenderType) :
Component(),
_plugGUI(plugGUI),
_detailGraphs(detailGraphs),
_getCurrentRenderType(getCurrentRenderType),
_fixedToRuntimeModuleIndex(fixedToRuntimeModuleIndex),
_fixedToGraphIndex(fixedToGraphIndex),
_data(std::make_unique<FBModuleGraphComponentData>())
{
  _data->renderState = renderState;
}

void
FBModuleGraphComponent::resized()
{
  if (_grid)
  {
    _grid->setBounds(getLocalBounds());
    _grid->resized();
  }
}

FBTopoIndices const&
FBModuleGraphComponent::TopoIndicesFor(int moduleIndex) const
{
  auto moduleProcState = _data->renderState->ModuleProcState();
  return moduleProcState->topo->modules[moduleIndex].topoIndices;
}

FBStaticModule const&
FBModuleGraphComponent::StaticModuleFor(int moduleIndex) const
{
  int staticIndex = TopoIndicesFor(moduleIndex).index;
  auto moduleProcState = _data->renderState->ModuleProcState();
  return moduleProcState->topo->static_->modules[staticIndex];
}

bool
FBModuleGraphComponent::PrepareForRender(int moduleIndex)
{
  if (moduleIndex == -1)
    return false;
  if (_fixedToRuntimeModuleIndex != -1)
    return moduleIndex == _fixedToRuntimeModuleIndex;
  auto moduleProcState = _data->renderState->ModuleProcState();
  moduleProcState->moduleSlot = TopoIndicesFor(moduleIndex).slot;
  auto const& staticTopo = StaticModuleFor(moduleIndex);
  return staticTopo.graphParticipatesInMain && staticTopo.graphRenderer != nullptr;
}

void
FBModuleGraphComponent::RequestRerender(int moduleIndex)
{
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;

  if (!PrepareForRender(moduleIndex))
    return;
  _tweakedModuleByUI = moduleIndex;

  auto const& staticTopo = StaticModuleFor(moduleIndex);
  int graphCount = _detailGraphs ? staticTopo.detailGraphCount : 1;
  bool detailGraphUpperLowerText = _detailGraphs ? staticTopo.detailGraphUpperLowerText : false;
  FBModuleGraphLayout layout = _detailGraphs ? staticTopo.detailGraphLayout : FBModuleGraphLayout::LeftToRight;
  if (_graphCount != graphCount || _layout != layout || _detailGraphUpperLowerText != detailGraphUpperLowerText)
  {
    _layout = layout;
    _graphCount = graphCount;
    _detailGraphUpperLowerText = detailGraphUpperLowerText;

    int rows = 0;
    int cols = 0;
    if (_fixedToRuntimeModuleIndex != -1)
    {
      rows = 1;
      cols = 1;
    }
    else if (_layout == FBModuleGraphLayout::LeftToRight)
    {
      rows = 1;
      cols = graphCount;
    }
    else if (_layout == FBModuleGraphLayout::TopToBottom)
    {
      cols = 1;
      rows = graphCount;
    }
    else if (_layout == FBModuleGraphLayout::Grid)
    {
      rows = (int)std::ceil(std::sqrt((float)graphCount));
      cols = (int)std::ceil(graphCount / (float)rows);
    }
    else
      FB_ASSERT(false);

    _columnCount = cols;
    removeChildComponent(_grid.get());
    _grid = std::make_unique<FBGridComponent>(_plugGUI, true, rows, cols);
    _cards.clear();
    _displays.clear();
    _upperTexts.clear();
    _lowerTexts.clear();
    _displayAndTextGrids.clear();

    int r = 0;
    int c = 0;
    for (int i = 0; i < graphCount; i++)
    {
      std::unique_ptr<FBGridComponent> displayAndTextGrid = {};
      auto display = std::make_unique<FBModuleGraphDisplayComponent>(_plugGUI, _data.get(), i);
      if (_fixedToRuntimeModuleIndex != -1)
      {
        displayAndTextGrid = std::make_unique<FBGridComponent>(_plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 1 });
        displayAndTextGrid->Add(0, 0, display.get());
      }
      else if (detailGraphUpperLowerText)
      {
        auto upperText = std::make_unique<FBModuleGraphTextComponent>(_plugGUI, _data.get(), i, FBModuleGraphTextType::TitleGain);
        auto lowerText = std::make_unique<FBModuleGraphTextComponent>(_plugGUI, _data.get(), i, FBModuleGraphTextType::Main);
        displayAndTextGrid = std::make_unique<FBGridComponent>(_plugGUI, true, std::vector<int> { 0, 1, 0 }, std::vector<int> { 1 });
        displayAndTextGrid->Add(0, 0, upperText.get());
        displayAndTextGrid->Add(1, 0, display.get());
        displayAndTextGrid->Add(2, 0, lowerText.get());
        _upperTexts.emplace_back(std::move(upperText));
        _lowerTexts.emplace_back(std::move(lowerText));
      }
      else
      {
        auto text = std::make_unique<FBModuleGraphTextComponent>(_plugGUI, _data.get(), i, FBModuleGraphTextType::Both);
        displayAndTextGrid = std::make_unique<FBGridComponent>(_plugGUI, true, std::vector<int> { 1, 0 }, std::vector<int> { 1 });
        displayAndTextGrid->Add(0, 0, display.get());
        displayAndTextGrid->Add(1, 0, text.get());
        _lowerTexts.emplace_back(std::move(text));
      }

      auto card = std::make_unique<FBCardComponent>(_plugGUI, displayAndTextGrid.get());
      _grid->Add(r, c, _fixedToRuntimeModuleIndex != -1? (Component*)displayAndTextGrid.get(): card.get());
      _cards.emplace_back(std::move(card));
      _displays.emplace_back(std::move(display));
      _displayAndTextGrids.emplace_back(std::move(displayAndTextGrid));

      c++;
      if (c == cols)
      {
        c = 0;
        r++;
      }
    }
    addAndMakeVisible(_grid.get());
    resized();
  }

  float fps = FBGUIFPS;
  auto now = high_resolution_clock::now();
  auto elapsedMillis = duration_cast<milliseconds>(now - _updated);
  if (_getCurrentRenderType() == FBGUIRenderType::Basic)
    fps = 1;
  if (elapsedMillis.count() < 1000.0 / fps)
    return;
  _updated = now;
  repaint();
}

void
FBModuleGraphComponent::paint(Graphics& /*g*/)
{
  if (_tweakedModuleByUI == -1)
    return;
  if (!PrepareForRender(_tweakedModuleByUI))
    return;

  auto const* topo = _data->renderState->ModuleProcState()->topo;
  int staticIndex = topo->modules[_tweakedModuleByUI].topoIndices.index;
  FB_ASSERT(topo->static_->modules[staticIndex].graphParticipatesInMain == (_fixedToRuntimeModuleIndex == -1));

  _data->drawMarkersSelector = {};
  _data->skipDrawOnEqualsPrimary = true;
  _data->fixedGraphIndex = _fixedToGraphIndex;
  _data->guiRenderType = _getCurrentRenderType();

  _data->graphs.clear();
  _data->graphs.resize(_detailGraphs? topo->static_->modules[staticIndex].detailGraphCount: 1);
  _data->pixelWidth = getWidth() / _columnCount;
  topo->static_->modules[staticIndex].graphRenderer(_data.get(), _detailGraphs);
}