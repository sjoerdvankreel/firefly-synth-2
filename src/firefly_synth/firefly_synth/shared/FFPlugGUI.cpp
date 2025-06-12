#include <firefly_synth/shared/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/effect/FFEffectGUI.hpp>
#include <firefly_synth/modules/master/FFMasterGUI.hpp>
#include <firefly_synth/modules/output/FFOutputGUI.hpp>
#include <firefly_synth/modules/shared/FFOscisGUI.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModGUI.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponent.hpp>

using namespace juce;

FFPlugGUI::
~FFPlugGUI() {}

FFPlugGUI::
FFPlugGUI(FBHostGUIContext* hostContext):
FBPlugGUI(hostContext),
_graphRenderState(std::make_unique<FBGraphRenderState>(this))
{
  FB_LOG_ENTRY_EXIT();
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
  float normalized = static_cast<float>(HostContext()->GetGUIParamNormalized(paramTopo->runtimeParamIndex));
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
  FB_LOG_ENTRY_EXIT();

  // TODO better move to reusable?
  FB_LOG_INFO("Calculating GUI grid size.");
  int vTabCount = 6;
  float tabHeight = 28.0f; // TODO
  auto const& topo = HostContext()->Topo()->static_;
  int totalHeight = topo.guiWidth * topo.guiAspectRatioHeight / topo.guiAspectRatioWidth;
  float availableHeight = static_cast<float>(totalHeight - vTabCount * tabHeight);
  std::vector<int> rowSizes = {};
  rowSizes.push_back(static_cast<int>(tabHeight + 1.0f / 9.0f * availableHeight));
  rowSizes.push_back(static_cast<int>(tabHeight + 1.0f / 9.0f * availableHeight));
  rowSizes.push_back(static_cast<int>(tabHeight + 2.0f / 9.0f * availableHeight));
  rowSizes.push_back(static_cast<int>(tabHeight + 2.0f / 9.0f * availableHeight));
  rowSizes.push_back(static_cast<int>(tabHeight + 2.0f / 9.0f * availableHeight));
  rowSizes.push_back(static_cast<int>(tabHeight + 2.0f / 9.0f * availableHeight));
  FB_LOG_INFO("Calculated GUI grid size.");

  FB_LOG_INFO("Creating GUI components.");
  _graph = StoreComponent<FBModuleGraphComponent>(this, _graphRenderState.get());
  _content = StoreComponent<FBGridComponent>(FBGridType::Generic, 1, -1, rowSizes, std::vector<int> { 1, 0, 0 });
  _content->Add(0, 0, 1, 3, _graph);
  _content->Add(1, 0, 1, 1, FFMakeMasterGUI(this));
  _content->Add(1, 1, 1, 1, FFMakeOutputGUI(this));
  _content->Add(1, 2, 1, 1, FFMakeGUISettingsGUI(this));
  _content->Add(2, 0, 1, 3, FFMakeOscisGUI(this));
  _content->Add(3, 0, 1, 3, FFMakeOsciModGUI(this));
  _content->Add(4, 0, 1, 3, FFMakeEnvGUI(this));
  _content->Add(5, 0, 1, 3, FFMakeEffectGUI(this));
  FB_LOG_INFO("Created GUI components.");

  FB_LOG_INFO("Making GUI visible.");
  addAndMakeVisible(_content);
  FB_LOG_INFO("Made GUI visible.");
}