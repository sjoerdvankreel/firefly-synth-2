#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/controls/FBAutoSizeSlider.hpp>
#include <playground_base/gui/controls/FBAutoSizeToggleButton.hpp>

#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>
#include <playground_base/gui/components/FBModuleGraphDisplayComponent.hpp>

using namespace juce;

FBModuleGraphComponent::
~FBModuleGraphComponent() {}

FBModuleGraphComponent::
FBModuleGraphComponent(FBPlugGUI* plugGUI, FBGraphRenderState* renderState) :
Component(),
_plugGUI(plugGUI),
_data(std::make_unique<FBModuleGraphComponentData>()),
_display(std::make_unique<FBModuleGraphDisplayComponent>(_data.get()))
{
  _data->renderState = renderState;
}

void
FBModuleGraphComponent::resized()
{
  if (!_section)
    return;
  _section->setBounds(getLocalBounds());
  _section->resized();
}

void
FBModuleGraphComponent::RequestRerender(int moduleIndex)
{
  if (!PrepareForRender(moduleIndex))
    return;
  if (_tweakedModuleByUI != moduleIndex)
  {
    _tweakedModuleByUI = moduleIndex;
    SetupGraphControls();
  }
  repaint();
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
  return moduleProcState->topo->static_.modules[staticIndex];
}

bool 
FBModuleGraphComponent::PrepareForRender(int moduleIndex)
{
  if (moduleIndex == -1)
    return false;
  auto moduleProcState = _data->renderState->ModuleProcState();
  moduleProcState->moduleSlot = TopoIndicesFor(moduleIndex).slot;
  return StaticModuleFor(moduleIndex).graphRenderer != nullptr;
}

void
FBModuleGraphComponent::paint(Graphics& g)
{
  if (_tweakedModuleByUI == -1)
    return;
  if (!PrepareForRender(_tweakedModuleByUI))
    return;

  auto const* topo = _data->renderState->ModuleProcState()->topo;
  int staticIndex = topo->modules[_tweakedModuleByUI].topoIndices.index;

  _data->bipolar = false;
  _data->drawMarkers = false;
  _data->drawClipBoundaries = false;
  _data->skipDrawOnEqualsPrimary = true;
  _data->series.clear();
  _data->series.resize(topo->static_.modules[staticIndex].graphCount);
  _data->pixelWidth = getWidth() / static_cast<int>(_data->series.size());
  for(int s = 0; s < _data->series.size(); s++)
    _data->series[s].moduleName = topo->modules[_tweakedModuleByUI].name; // TODO
  topo->static_.modules[staticIndex].graphRenderer(_data.get());
}

void
FBModuleGraphComponent::SetupGraphControls()
{
  removeChildComponent(_section.get());
  _graphControls = _plugGUI->GetGraphControlsForModule(_tweakedModuleByUI);

  if (_graphControls == nullptr)
  {
    _grid = std::make_unique<FBGridComponent>(FBGridType::Module, 1, 1);
    _grid->Add(0, 0, _display.get());
    _grid->MarkSection({ 0, 0, 1, 1 });
  }
  else
  {
    _grid = std::make_unique<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
    _grid->Add(0, 0, _graphControls);
    _grid->Add(0, 1, _display.get());
    _grid->MarkSection({ 0, 0, 1, 1 });
    _grid->MarkSection({ 0, 1, 1, 1 });
  }

  _section = std::make_unique<FBSectionComponent>(_grid.get());
  addAndMakeVisible(_section.get());
  resized();
}