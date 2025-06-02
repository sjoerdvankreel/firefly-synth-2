#include <playground_plug/shared/FFPlugGUI.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/effect/FFEffectGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/output/FFOutputGUI.hpp>
#include <playground_plug/modules/physical/FFPhysGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModGUI.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
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
  _graph->RequestRerender(_graph->TweakedModuleByUI());
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
  _graph->RequestRerender(topo->runtimeModuleIndex);
}

void 
FFPlugGUI::GUIParamNormalizedChanged(int index, double normalized)
{
  FBPlugGUI::GUIParamNormalizedChanged(index, normalized);
  int moduleIndex = HostContext()->Topo()->gui.params[index].runtimeModuleIndex;
  _graph->RequestRerender(moduleIndex);
}

void 
FFPlugGUI::AudioParamNormalizedChangedFromUI(int index, double normalized)
{
  FBPlugGUI::AudioParamNormalizedChangedFromUI(index, normalized);
  int moduleIndex = HostContext()->Topo()->audio.params[index].runtimeModuleIndex;
  _graph->RequestRerender(moduleIndex);
}

void
FFPlugGUI::AudioParamNormalizedChangedFromHost(int index, double normalized)
{
  FBPlugGUI::AudioParamNormalizedChangedFromHost(index, normalized);
  if (HostContext()->Topo()->audio.params[index].static_.output)
    return;
  if (_graph->TweakedModuleByUI() == HostContext()->Topo()->audio.params[index].runtimeModuleIndex)
    _graph->RequestRerender(_graph->TweakedModuleByUI());
}

FBGUIRenderType 
FFPlugGUI::GetRenderType() const
{
  FBParamTopoIndices indices = { (int)FFModuleType::GUISettings, 0, (int)FFGUISettingsGUIParam::GraphMode, 0 };
  auto const* paramTopo = HostContext()->Topo()->gui.ParamAtTopo(indices);
  auto normalized = HostContext()->GetGUIParamNormalized(paramTopo->runtimeParamIndex);
  auto mode = static_cast<FFGUISettingsGraphMode>(paramTopo->static_.List().NormalizedToPlainFast(normalized));
  if (mode == FFGUISettingsGraphMode::Basic)
    return FBGUIRenderType::Basic;
  if (mode == FFGUISettingsGraphMode::Always)
    return FBGUIRenderType::Full;
  return hasKeyboardFocus(true) ? FBGUIRenderType::Full : FBGUIRenderType::Basic;
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
  rowSizes.push_back(tabHeight + 2.0f / 9.0f * availableHeight);
  rowSizes.push_back(tabHeight + 2.0f / 9.0f * availableHeight);

  _graph = StoreComponent<FBModuleGraphComponent>(this, _graphRenderState.get());
  _content = StoreComponent<FBGridComponent>(FBGridType::Generic, 0, -1, rowSizes, std::vector<int> { 0, 0, 0, 1 });
  _content->Add(0, 0, 1, 1, FFMakeMasterGUI(this));
  _content->Add(0, 1, 1, 1, FFMakeOutputGUI(this));
  _content->Add(0, 2, 1, 1, FFMakeGUISettingsGUI(this));
  _content->Add(0, 3, 1, 1, _graph);
  _content->Add(1, 0, 1, 4, FFMakeGLFOGUI(this));
  _content->Add(2, 0, 1, 4, FFMakeGFilterGUI(this));
  _content->Add(3, 0, 1, 4, FFMakePhysGUI(this));
  _content->Add(4, 0, 1, 4, FFMakeOsciGUI(this));
  _content->Add(5, 0, 1, 4, FFMakeOsciModGUI(this));
  _content->Add(6, 0, 1, 4, FFMakeEffectGUI(this));
  _content->Add(7, 0, 1, 4, FFMakeEnvGUI(this));
  addAndMakeVisible(_content);
}