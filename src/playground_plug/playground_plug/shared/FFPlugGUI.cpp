#include <playground_plug/shared/FFPlugGUI.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

using namespace juce;

FFPlugGUI::
~FFPlugGUI() {}

FFPlugGUI::
FFPlugGUI(FBHostGUIContext* hostContext):
FBPlugGUI(hostContext),
_graphRenderState(std::make_unique<FBGraphRenderState>(this))
{
  SetupGUI();
  InitAllDependencies();
  resized();
}

void
FFPlugGUI::RequestGraphRender(int moduleIndex)
{
  _graph->RequestRerender(moduleIndex);
}

void
FFPlugGUI::UpdateExchangeStateTick()
{
  FBPlugGUI::UpdateExchangeStateTick();
  RequestGraphRender(_graph->TweakedModuleByUI());
}

void
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

void 
FFPlugGUI::ActiveModuleSlotChanged(int index, int slot)
{
  auto topo = HostContext()->Topo()->ModuleAtTopo({ index, slot });
  RequestGraphRender(topo->runtimeModuleIndex);
}

void 
FFPlugGUI::SetParamNormalizedFromUI(int index, float normalized)
{
  int moduleIndex = HostContext()->Topo()->params[index].runtimeModuleIndex;
  _graphRenderState->PrimaryParamChanged(index, normalized);
  RequestGraphRender(moduleIndex);
}

void
FFPlugGUI::SetParamNormalizedFromHost(int index, float normalized)
{
  FBPlugGUI::SetParamNormalizedFromHost(index, normalized);
  if (HostContext()->Topo()->params[index].static_.output)
    return;
  _graphRenderState->PrimaryParamChanged(index, normalized);
  if (_graph->TweakedModuleByUI() == HostContext()->Topo()->params[index].runtimeModuleIndex)
    RequestGraphRender(_graph->TweakedModuleByUI());
}

void 
FFPlugGUI::SetupGUI()
{
  _graph = StoreComponent<FBModuleGraphComponent>(this, _graphRenderState.get());
  _content = StoreComponent<FBGridComponent>(FBGridType::Generic, std::vector<int> { 1, 1, 1, 1, 2 }, std::vector<int> { 1, 1 });
  _content->Add(0, 0, 1, 1, FFMakeMasterGUI(this));
  _content->Add(0, 1, 1, 1, _graph);
  _content->Add(1, 0, 1, 2, FFMakeGLFOGUI(this));
  _content->Add(2, 0, 1, 2, FFMakeGFilterGUI(this));
  _content->Add(3, 0, 1, 2, FFMakeOsciGUI(this));
  _content->Add(4, 0, 1, 2, FFMakeEnvGUI(this));
  addAndMakeVisible(_content);
}