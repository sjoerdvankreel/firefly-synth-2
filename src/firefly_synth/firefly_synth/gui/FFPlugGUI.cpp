#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/gui/FFPatchGUI.hpp>
#include <firefly_synth/gui/FFHeaderGUI.hpp>
#include <firefly_synth/gui/FFPlugMatrixGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/lfo/FFLFOGUI.hpp>
#include <firefly_synth/modules/mix/FFMixGUI.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/effect/FFEffectGUI.hpp>
#include <firefly_synth/modules/master/FFMasterGUI.hpp>
#include <firefly_synth/modules/output/FFOutputGUI.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
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
FFPlugGUI::SwitchGraphToModule(int index, int slot)
{
  auto topo = HostContext()->Topo()->ModuleAtTopo({ index, slot });
  _graph->RequestRerender(topo->runtimeModuleIndex);
}

void 
FFPlugGUI::ModuleSlotClicked(int index, int slot)
{
  SwitchGraphToModule(index, slot);
}

void
FFPlugGUI::ActiveModuleSlotChanged(int index, int slot)
{
  SwitchGraphToModule(index, slot);
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
FFPlugGUI::GetKnobRenderType() const
{
  return GetRenderType((int)FFGUISettingsGUIParam::KnobRenderMode);
}

FBGUIRenderType
FFPlugGUI::GetGraphRenderType() const
{
  return GetRenderType((int)FFGUISettingsGUIParam::GraphRenderMode);
}

FBGUIRenderType 
FFPlugGUI::GetRenderType(int paramIndex) const
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0 }, { paramIndex, 0 } };
  auto const* paramTopo = HostContext()->Topo()->gui.ParamAtTopo(indices);
  float normalized = static_cast<float>(HostContext()->GetGUIParamNormalized(paramTopo->runtimeParamIndex));
  auto mode = static_cast<FFGUISettingsRenderMode>(paramTopo->static_.List().NormalizedToPlainFast(normalized));
  if (mode == FFGUISettingsRenderMode::Basic)
    return FBGUIRenderType::Basic;
  if (mode == FFGUISettingsRenderMode::Always)
    return FBGUIRenderType::Full;
  return hasKeyboardFocus(true) ? FBGUIRenderType::Full : FBGUIRenderType::Basic;
}

bool
FFPlugGUI::ToggleMatrix()
{
  if (_showMatrix)
    _content->SetContent(_modules);
  else
    _content->SetContent(_matrix);
  _showMatrix = !_showMatrix;
  return _showMatrix;
}

void 
FFPlugGUI::SetupGUI()
{
  FB_LOG_ENTRY_EXIT();
  _matrix = FFMakeModMatrixGUI(this); 
  _graph = StoreComponent<FBModuleGraphComponent>(_graphRenderState.get());
  _modules = StoreComponent<FBGridComponent>(false, 1, -1, std::vector<int>(5, 1), std::vector<int> { { 1 } });
  _modules->Add(0, 0, FFMakeOsciGUI(this));
  _modules->Add(1, 0, FFMakeEffectGUI(this));
  _modules->Add(2, 0, FFMakeLFOGUI(this));
  _modules->Add(3, 0, FFMakeEnvGUI(this));
  _modules->Add(4, 0, FFMakeMixGUI(this));
  _content = StoreComponent<FBContentComponent>();
  _content->SetContent(_modules);
  _container = StoreComponent<FBGridComponent>(false, 1, -1, std::vector<int> { { 1, 1, 7 } }, std::vector<int> { { 0, 1, 0, 0, 0, 0 } });
  _container->Add(0, 0, 1, 6, _graph);
  _container->Add(1, 0, 1, 1, FFMakeHeaderGUI(this));
  _container->Add(1, 1, 1, 1, FFMakeMasterGUI(this));
  _container->Add(1, 2, 1, 1, FFMakeOutputGUI(this));
  _container->Add(1, 3, 1, 1, FFMakeGUISettingsGUI(this));
  _container->Add(1, 4, 1, 1, FFMakePlugMatrixGUI(this));
  _container->Add(1, 5, 1, 1, FFMakePatchGUI(this));
  _container->Add(2, 0, 1, 6, _content);
  addAndMakeVisible(_container);
}