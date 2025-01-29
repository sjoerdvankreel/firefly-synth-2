#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(FBRuntimeTopo const* topo, FBHostGUIContext* hostContext):
FBPlugGUI(topo, hostContext),
_graphProcState(this)
{
  SetupGUI();
  InitAllDependencies();
  resized();
}

void
FFPlugGUI::RequestGraphRender(int paramIndex)
{
  _graphProcState.PrepareForRender();
  _graph->RequestRerender(paramIndex);
}

void
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

void 
FFPlugGUI::ParamNormalizedChangedFromUI(int index)
{
  float normalized = HostContext()->GetParamNormalized(index);
  _graphProcState.container.InitProcessing(index, normalized);
  RequestGraphRender(index);
}

void
FFPlugGUI::SetParamNormalizedFromHost(int index, float normalized)
{
  FBPlugGUI::SetParamNormalizedFromHost(index, normalized);
  _graphProcState.container.InitProcessing(index, normalized);
  if (_graph->TweakedParamByUI() != -1 &&
    Topo()->params[index].runtimeModuleIndex ==
    Topo()->params[_graph->TweakedParamByUI()].runtimeModuleIndex)
    RequestGraphRender(index);
}

void 
FFPlugGUI::SetupGUI()
{
  _graph = StoreComponent<FBModuleGraphComponent>(&_graphProcState);
  _content = StoreComponent<FBGridComponent>(FBGridType::Generic, std::vector<int> { 1, 1, 1, 1, 2 }, std::vector<int> { 1, 1 });
  _content->Add(0, 0, 1, 1, FFMakeMasterGUI(this));
  _content->Add(0, 1, 1, 1, _graph);
  _content->Add(1, 0, 1, 2, FFMakeGLFOGUI(this));
  _content->Add(2, 0, 1, 2, FFMakeGFilterGUI(this));
  _content->Add(3, 0, 1, 2, FFMakeOsciGUI(this));
  _content->Add(4, 0, 1, 2, FFMakeEnvGUI(this));
  addAndMakeVisible(_content);
  addAndMakeVisible(StoreComponent<TooltipWindow>());
}