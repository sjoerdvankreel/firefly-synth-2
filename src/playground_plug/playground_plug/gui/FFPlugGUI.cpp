#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/gui/FFGUIConfig.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
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
FFPlugGUI::RequestGraphRender(int paramIndex)
{
  _graph->RequestRerender(paramIndex);
}

void
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

void 
FFPlugGUI::SetParamNormalizedFromUI(int index, float normalized)
{
  _graphRenderState->PrimaryParamChanged(index, normalized);
  RequestGraphRender(index);
}

void
FFPlugGUI::SetParamNormalizedFromHost(int index, float normalized)
{
  FBPlugGUI::SetParamNormalizedFromHost(index, normalized);
  if (HostContext()->Topo()->params[index].static_.output)
    return;
  _graphRenderState->PrimaryParamChanged(index, normalized);
  if (_graph->TweakedParamByUI() != -1 &&
    HostContext()->Topo()->params[index].runtimeModuleIndex ==
    HostContext()->Topo()->params[_graph->TweakedParamByUI()].runtimeModuleIndex)
    RequestGraphRender(index);
}

void
FFPlugGUI::UpdateExchangeState()
{
  auto now = std::chrono::high_resolution_clock::now();
  auto elapsedMillis = std::chrono::duration_cast<std::chrono::milliseconds>(now - _exchangeUpdated);
  if (elapsedMillis.count() < 1000.0 / FFGUIFPS)
    return;
  _exchangeUpdated = now;
  if (_graph->TweakedParamByUI() != -1)
    RequestGraphRender(_graph->TweakedParamByUI());
}

void 
FFPlugGUI::SetupGUI()
{
  _graph = StoreComponent<FBModuleGraphComponent>(_graphRenderState.get());
  _content = StoreComponent<FBGridComponent>(FBGridType::Generic, std::vector<int> { 1, 1, 1, 1, 2 }, std::vector<int> { 1, 1 });
  _content->Add(0, 0, 1, 1, FFMakeMasterGUI(this));
  _content->Add(0, 1, 1, 1, _graph);
  _content->Add(1, 0, 1, 2, FFMakeGLFOGUI(this));
  _content->Add(2, 0, 1, 2, FFMakeGFilterGUI(this));
  _content->Add(3, 0, 1, 2, FFMakeOsciGUI(this));
  _content->Add(4, 0, 1, 2, FFMakeEnvGUI(this));
  addAndMakeVisible(_content);
}