#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
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
_controlActive(),
_grid(std::make_unique<FBGridComponent>(FBGridType::Generic, 1, std::vector<int> { 1, 3 })), // TODO 1/3
_data(std::make_unique<FBModuleGraphComponentData>()),
_display(std::make_unique<FBModuleGraphDisplayComponent>(_data.get()))
{
  _data->renderState = renderState;
  _grid->Add(0, 0, &_controlActive);
  _grid->Add(0, 1, _display.get());
  addAndMakeVisible(_grid.get());
}

void
FBModuleGraphComponent::resized()
{
  _grid->setBounds(getLocalBounds());
  _grid->resized();
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
  return StaticModuleFor(moduleIndex).renderGraph != nullptr;
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

  _data->text.clear();
  _data->primarySeries.clear();
  _data->primaryMarkers.clear();
  _data->secondarySeries.clear();
  _data->pixelWidth = getWidth();
  _data->moduleName = topo->modules[_tweakedModuleByUI].name;
  topo->static_.modules[staticIndex].renderGraph(_data.get());
}

void
FBModuleGraphComponent::RequestRerender(int moduleIndex)
{
  if (!PrepareForRender(moduleIndex))
    return;
  if (_tweakedModuleByUI != moduleIndex)
  {
    _tweakedModuleByUI = moduleIndex;
    if (_graphControl != nullptr)
      removeChildComponent(_graphControl.get());
    auto const& staticModule = StaticModuleFor(_tweakedModuleByUI);
    if (staticModule.graphControlParam != -1)
      _graphControl = MakeGraphControl(staticModule.graphControlParam);
    resized();
  }
  repaint();
}

std::unique_ptr<FBParamSlider>
FBModuleGraphComponent::MakeGraphControl(int graphControlParamIndex) const
{
  auto const* topo = _data->renderState->ModuleProcState()->topo;
  int staticIndex = TopoIndicesFor(_tweakedModuleByUI).index;
  FBParamTopoIndices indices = { staticIndex, 0, graphControlParamIndex, 0 };
  return std::make_unique<FBParamSlider>(_plugGUI, topo->ParamAtTopo(indices), Slider::SliderStyle::RotaryVerticalDrag);
}