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
  // TODO colspan & friends
  if (_graphControl.get() != nullptr)
    _grid->Remove(_graphControl.get());

  _grid->setBounds(getLocalBounds());
  _grid->resized();
}

bool 
FBModuleGraphComponent::PrepareForRender(int moduleIndex)
{
  if (moduleIndex == -1)
    return false;
  auto moduleProcState = _data->renderState->ModuleProcState();
  auto const& topoIndices = moduleProcState->topo->modules[moduleIndex].topoIndices;
  auto const& staticModule = moduleProcState->topo->static_.modules[topoIndices.index];
  moduleProcState->moduleSlot = topoIndices.slot;
  return staticModule.renderGraph != nullptr;
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
    if (_graphControl)
      removeChildComponent(dynamic_cast<Component*>(_graphControl.get()));
    auto const* topo = _data->renderState->ModuleProcState()->topo;
    int staticIndex = topo->modules[_tweakedModuleByUI].topoIndices.index;
    auto const& staticModule = topo->static_.modules[staticIndex];
    if (staticModule.graphControlParam != -1)
    {
      FBParamTopoIndices indices = { staticIndex, 0, staticModule.graphControlParam, 0 };
      auto const* runtimeControlParam = topo->ParamAtTopo(indices);
      _graphControl = std::make_unique<FBParamSlider>(_plugGUI, runtimeControlParam, Slider::SliderStyle::LinearHorizontal);
    }
    resized();
  }
  repaint();
}