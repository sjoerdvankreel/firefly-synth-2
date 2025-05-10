#include <playground_plug/shared/FFPlugGUI.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsTopo.hpp>

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
FFPlugGUI::GUIParamNormalizedChanged(int index, double normalized)
{
  FBPlugGUI::GUIParamNormalizedChanged(index, normalized);
  int moduleIndex = HostContext()->Topo()->gui.params[index].runtimeModuleIndex;
  RequestGraphRender(moduleIndex);
}

void 
FFPlugGUI::AudioParamNormalizedChangedFromUI(int index, double normalized)
{
  FBPlugGUI::AudioParamNormalizedChangedFromUI(index, normalized);
  int moduleIndex = HostContext()->Topo()->audio.params[index].runtimeModuleIndex;
  RequestGraphRender(moduleIndex);
}

void
FFPlugGUI::AudioParamNormalizedChangedFromHost(int index, double normalized)
{
  FBPlugGUI::AudioParamNormalizedChangedFromHost(index, normalized);
  if (HostContext()->Topo()->audio.params[index].static_.output)
    return;
  if (_graph->TweakedModuleByUI() == HostContext()->Topo()->audio.params[index].runtimeModuleIndex)
    RequestGraphRender(_graph->TweakedModuleByUI());
}

void
FFPlugGUI::RequestGraphRender(int moduleIndex)
{
  FBParamTopoIndices indices = { (int)FFModuleType::GUISettings, 0, (int)FFGUISettingsGUIParam::GraphTrack, 0 };
  auto const* param = HostContext()->Topo()->gui.ParamAtTopo(indices);
  double norm = HostContext()->GetGUIParamNormalized(param->runtimeParamIndex);
  bool graphTrack = param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(norm));
  _graph->RequestRerender(graphTrack ? moduleIndex : _graph->TweakedModuleByUI());
}

void 
FFPlugGUI::SetupGUI()
{
  // TODO better move to reusable?
  int vTabCount = 6;
  int tabHeight = 28; // TODO
  auto const& topo = HostContext()->Topo()->static_;
  int totalHeight = topo.guiWidth * topo.guiAspectRatioHeight / topo.guiAspectRatioWidth;
  float availableHeight = totalHeight - vTabCount * tabHeight;
  std::vector<int> rowSizes = {};
  rowSizes.push_back(tabHeight + 1.0f / 9.0f * availableHeight);
  rowSizes.push_back(tabHeight + 1.0f / 9.0f * availableHeight);
  rowSizes.push_back(tabHeight + 1.0f / 9.0f * availableHeight);
  rowSizes.push_back(tabHeight + 2.0f / 9.0f * availableHeight);
  rowSizes.push_back(tabHeight + 2.0f / 9.0f * availableHeight);
  rowSizes.push_back(tabHeight + 2.0f / 9.0f * availableHeight);

  _graph = StoreComponent<FBModuleGraphComponent>(this, _graphRenderState.get());
  _content = StoreComponent<FBGridComponent>(FBGridType::Generic, 0, -1, rowSizes, std::vector<int> { 0, 0, 1 });
  _content->Add(0, 0, 1, 1, FFMakeMasterGUI(this));
  _content->Add(0, 1, 1, 1, FFMakeGUISettingsGUI(this));
  _content->Add(0, 2, 1, 1, _graph);
  _content->Add(1, 0, 1, 3, FFMakeGLFOGUI(this));
  _content->Add(2, 0, 1, 3, FFMakeGFilterGUI(this));
  _content->Add(3, 0, 1, 3, FFMakeOsciGUI(this));
  _content->Add(4, 0, 1, 3, FFMakeOsciModGUI(this));
  _content->Add(5, 0, 1, 3, FFMakeEnvGUI(this));
  addAndMakeVisible(_content);
}