#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>

#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/components/FBModuleGraphTitleComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphDisplayComponent.hpp>

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
  if (_graphCount != graphCount)
  {
    _graphCount = graphCount;
    removeChildComponent(_grid.get());
    _grid = std::make_unique<FBGridComponent>(_plugGUI, true, 1, _graphCount);
    _cards.clear();
    _titles.clear();
    _displays.clear();
    _displayAndTitleGrids.clear();
    for (int i = 0; i < graphCount; i++)
    {
      auto title = std::make_unique<FBModuleGraphTitleComponent>(_plugGUI, _data.get(), i);
      auto display = std::make_unique<FBModuleGraphDisplayComponent>(_plugGUI, _data.get(), i);
      auto displayAndTitleGrid = std::make_unique<FBGridComponent>(_plugGUI, true, std::vector<int> { 1, 0 }, std::vector<int> { 1 });
      displayAndTitleGrid->Add(0, 0, display.get());
      displayAndTitleGrid->Add(1, 0, title.get());
      auto card = std::make_unique<FBCardComponent>(_plugGUI, displayAndTitleGrid.get());
      _grid->Add(0, i, card.get());
      _cards.emplace_back(std::move(card));
      _titles.emplace_back(std::move(title));
      _displays.emplace_back(std::move(display));
      _displayAndTitleGrids.emplace_back(std::move(displayAndTitleGrid));
    }
    addAndMakeVisible(_grid.get());
    resized();
  }

  float fps = FBGUIFPS;
  auto now = high_resolution_clock::now();
  auto elapsedMillis = duration_cast<milliseconds>(now - _updated);
  if (_getCurrentRenderType() == FBGUIRenderType::Basic) // todo get rid of it
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
  _data->pixelWidth = getWidth() / static_cast<int>(_data->graphs.size());
  topo->static_->modules[staticIndex].graphRenderer(_data.get(), _detailGraphs);
}