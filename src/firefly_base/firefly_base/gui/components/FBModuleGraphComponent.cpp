#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>

#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>

#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/components/FBModuleGraphDisplayComponent.hpp>

using namespace juce;

FBModuleGraphComponent::
~FBModuleGraphComponent() {}

FBModuleGraphComponent::
FBModuleGraphComponent(FBGraphRenderState* renderState) :
Component(),
_data(std::make_unique<FBModuleGraphComponentData>()),
_display(std::make_unique<FBModuleGraphDisplayComponent>(_data.get()))
{
  _data->renderState = renderState;
  _grid = std::make_unique<FBGridComponent>(true, 1, 1);
  _grid->Add(0, 0, _display.get());
  _section = std::make_unique<FBSectionComponent>(_grid.get());
  addAndMakeVisible(_section.get());
  resized();
}

void
FBModuleGraphComponent::resized()
{
  if (!_section)
    return;
  _section->setBounds(getLocalBounds());
  _section->resized();
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
FBModuleGraphComponent::RequestRerender(int moduleIndex)
{
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;

  if (!PrepareForRender(moduleIndex))
    return;
  _tweakedModuleByUI = moduleIndex;

  float fps = FBGUIFPS;
  auto now = high_resolution_clock::now();
  auto elapsedMillis = duration_cast<milliseconds>(now - _updated);
  if (auto* parent = findParentComponentOfClass<FBPlugGUI>())
    if(parent->GetRenderType() == FBGUIRenderType::Basic)
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

  _data->bipolar = false;
  _data->drawMarkersSelector = {};
  _data->drawClipBoundaries = false;
  _data->skipDrawOnEqualsPrimary = true;
  _data->guiRenderType = FBGUIRenderType::Basic;
  if(auto* parent = findParentComponentOfClass<FBPlugGUI>())
    _data->guiRenderType = parent->GetRenderType();

  _data->graphs.clear();
  _data->graphs.resize(topo->static_.modules[staticIndex].graphCount);
  _data->pixelWidth = getWidth() / static_cast<int>(_data->graphs.size());
  topo->static_.modules[staticIndex].graphRenderer(_data.get());
}