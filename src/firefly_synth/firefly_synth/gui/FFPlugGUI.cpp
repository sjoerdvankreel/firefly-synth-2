#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/gui/FFPatchGUI.hpp>
#include <firefly_synth/gui/FFTweakGUI.hpp>
#include <firefly_synth/gui/FFHeaderGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/lfo/FFLFOGUI.hpp>
#include <firefly_synth/modules/mix/FFMixGUI.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/echo/FFEchoGUI.hpp>
#include <firefly_synth/modules/effect/FFEffectGUI.hpp>
#include <firefly_synth/modules/master/FFMasterGUI.hpp>
#include <firefly_synth/modules/output/FFOutputGUI.hpp>
#include <firefly_synth/modules/output/FFOutputTopo.hpp>
#include <firefly_synth/modules/settings/FFSettingsGUI.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGUI.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleGUI.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponent.hpp>

using namespace juce;

FFPlugGUI::
~FFPlugGUI() { }

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
FFPlugGUI::OnPatchChanged()
{
  FlushAudio();
}

void 
FFPlugGUI::RequestFixedGraphsRerender(int moduleIndex)
{
  for (int i = 0; i < _fixedGraphs.size(); i++)
    _fixedGraphs[i]->RequestRerender(
      moduleIndex == -1? _fixedGraphs[i]->FixedToRuntimeModuleIndex(): moduleIndex);
}

void
FFPlugGUI::UpdateExchangeStateTick()
{
  FBPlugGUI::UpdateExchangeStateTick();
  _mainGraph->RequestRerender(_mainGraph->TweakedModuleByUI());
  RequestFixedGraphsRerender(-1);
}

void
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

void
FFPlugGUI::SwitchMainGraphToModule(int index, int slot)
{
  auto topo = HostContext()->Topo()->ModuleAtTopo({ index, slot });
  _mainGraph->RequestRerender(topo->runtimeModuleIndex);
}

void 
FFPlugGUI::ModuleSlotClicked(int index, int slot)
{
  SwitchMainGraphToModule(index, slot);
}

void
FFPlugGUI::ActiveModuleSlotChanged(int index, int slot)
{
  SwitchMainGraphToModule(index, slot);
}

void 
FFPlugGUI::GUIParamNormalizedChanged(int index, double normalized)
{
  FBPlugGUI::GUIParamNormalizedChanged(index, normalized);
  int moduleIndex = HostContext()->Topo()->gui.params[index].runtimeModuleIndex;
  _mainGraph->RequestRerender(moduleIndex);
  RequestFixedGraphsRerender(moduleIndex);
}

void 
FFPlugGUI::AudioParamNormalizedChangedFromUI(int index, double normalized)
{
  FBPlugGUI::AudioParamNormalizedChangedFromUI(index, normalized);
  int moduleIndex = HostContext()->Topo()->audio.params[index].runtimeModuleIndex;
  _mainGraph->RequestRerender(moduleIndex);
  RequestFixedGraphsRerender(moduleIndex);
}

void
FFPlugGUI::AudioParamNormalizedChangedFromHost(int index, double normalized)
{
  FBPlugGUI::AudioParamNormalizedChangedFromHost(index, normalized);
  if (HostContext()->Topo()->audio.params[index].static_.mode == FBParamMode::Output)
    return;
  int tweakedModule = HostContext()->Topo()->audio.params[index].runtimeModuleIndex;
  if (_mainGraph->TweakedModuleByUI() == tweakedModule)
    _mainGraph->RequestRerender(_mainGraph->TweakedModuleByUI());
  RequestFixedGraphsRerender(tweakedModule);
}

bool 
FFPlugGUI::HighlightTweaked() const
{
  FBParamTopoIndices indices = { { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsGUIParam::HilightTweak, 0 } };
  return HostContext()->GetGUIParamBool(indices);
}

FBGUIRenderType 
FFPlugGUI::GetRenderType(bool graphOrKnob) const
{
  FFSettingsGUIParam param = graphOrKnob ? FFSettingsGUIParam::GraphVisualsMode : FFSettingsGUIParam::KnobVisualsMode;
  FBParamTopoIndices indices = { { (int)FFModuleType::Settings, 0 }, { (int)param, 0 } };
  auto const* paramTopo = HostContext()->Topo()->gui.ParamAtTopo(indices);
  float normalized = static_cast<float>(HostContext()->GetGUIParamNormalized(paramTopo->runtimeParamIndex));
  auto mode = static_cast<FFSettingsVisualsMode>(paramTopo->static_.List().NormalizedToPlainFast(normalized));
  if (mode == FFSettingsVisualsMode::Basic)
    return FBGUIRenderType::Basic;
  if (mode == FFSettingsVisualsMode::Always)
    return FBGUIRenderType::Full;
  return hasKeyboardFocus(true) ? FBGUIRenderType::Full : FBGUIRenderType::Basic;
}

void
FFPlugGUI::FlushAudio()
{
  FBParamTopoIndices indices = { { (int)FFModuleType::Output, 0 }, { (int)FFOutputParam::FlushAudioToggle, 0 } };
  double flushNorm = HostContext()->GetAudioParamNormalized(indices);
  double newFlushNorm = flushNorm > 0.5 ? 0.0 : 1.0;
  HostContext()->PerformImmediateAudioParamEdit(indices, newFlushNorm);
}

void 
FFPlugGUI::SetupGUI()
{
  FB_LOG_ENTRY_EXIT();

  _mainGraph = StoreComponent<FBModuleGraphComponent>(_graphRenderState.get(), -1, -1, [this]() { return GetRenderType(true); });
  _headerAndGraph = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 0, 1 } });
  _headerAndGraph->Add(0, 0, FFMakeHeaderGUI(this));
  _headerAndGraph->Add(0, 1, _mainGraph);

  _outputTweakAndPatch = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 1, 0, 0 } });
  _outputTweakAndPatch->Add(0, 0, FFMakeOutputGUI(this));
  _outputTweakAndPatch->Add(0, 1, FFMakeTweakGUI(this));
  _outputTweakAndPatch->Add(0, 2, FFMakePatchGUI(this));

  _topModules = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 0, 0, 1 } });
  _topModules->Add(0, 0, FFMakeVoiceModuleGUI(this));
  _topModules->Add(0, 1, FFMakeMasterGUI(this));
  _topModules->Add(0, 2, FFMakeSettingsGUI(this));

  _matrix = FFMakeModMatrixGUI(this);
  _modMatrixParamListener = std::make_unique<FFModMatrixParamListener>(this);
  _globalUni = FFMakeGlobalUniGUI(this, _graphRenderState.get(), &_fixedGraphs);
  _globalUniParamListener = std::make_unique<FFGlobalUniParamListener>(this);
  _main = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int>(7, 1), std::vector<int> { { 1 } });
  _main->Add(0, 0, _topModules);
  _main->Add(1, 0, FFMakeMixGUI(this));
  _main->Add(2, 0, FFMakeOsciGUI(this));
  _main->Add(3, 0, FFMakeEffectGUI(this));
  _main->Add(4, 0, FFMakeEchoGUI(this));
  _main->Add(5, 0, FFMakeLFOGUI(this));
  _main->Add(6, 0, FFMakeEnvGUI(this));

  _tabs = StoreComponent<FBAutoSizeTabComponent>();
  _tabs->addTab("Main", Colours::black, _main, false);
  _tabs->addTab("Matrix", Colours::black, _matrix, false);
  _tabs->addTab("Unison", Colours::black, _globalUni, false);

  _container = StoreComponent<FBGridComponent>(false, 0, -1, std::vector<int> { { 6, 9, 92 } }, std::vector<int> { { 1 } });
  _container->Add(0, 0, _outputTweakAndPatch);
  _container->Add(1, 0, _headerAndGraph);
  _container->Add(2, 0, _tabs);
  addAndMakeVisible(_container);
}