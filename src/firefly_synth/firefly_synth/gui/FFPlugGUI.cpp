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
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGUI.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleGUI.hpp>

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

  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0}, {(int)FFGUISettingsGUIParam::HilightTweak, 0 } };
  if (index == HostContext()->Topo()->gui.ParamAtTopo(indices)->runtimeParamIndex)
    repaint();
  indices = { { (int)FFModuleType::GUISettings, 0}, {(int)FFGUISettingsGUIParam::HilightMod, 0 } };
  if (index == HostContext()->Topo()->gui.ParamAtTopo(indices)->runtimeParamIndex)
    repaint();
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
FFPlugGUI::GetParamModulationBounds(int index, double& minNorm, double& maxNorm) const
{
  // Note: we only take into account the matrix and the easy-access controls, not the unison.
  bool result = false;
  float valueNorm = (float)HostContext()->GetAudioParamNormalized(index);
  float currentMinNorm = valueNorm;
  float currentMaxNorm = valueNorm;    

  // Matrix goes first, as in DSP!
  result |= FFModMatrixAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm);
  result |= FFOsciAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm);
  result |= FFVMixAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm);
  result |= FFGMixAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm);
  result |= FFEffectAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm);
  result |= FFVoiceModuleAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm);
  minNorm = result? currentMinNorm: 0.0;
  maxNorm = result ? currentMaxNorm : 0.0f;
  return result;
}

bool
FFPlugGUI::HighlightModulationBounds() const
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightMod, 0 } };
  return HostContext()->GetGUIParamBool(indices);
}

bool 
FFPlugGUI::HighlightTweaked() const
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightTweak, 0 } };
  return HostContext()->GetGUIParamBool(indices);
}

FBGUIRenderType 
FFPlugGUI::GetRenderType(bool graphOrKnob) const
{
  FFGUISettingsGUIParam param = graphOrKnob ? FFGUISettingsGUIParam::GraphVisualsMode : FFGUISettingsGUIParam::KnobVisualsMode;
  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0 }, { (int)param, 0 } };
  auto const* paramTopo = HostContext()->Topo()->gui.ParamAtTopo(indices);
  float normalized = static_cast<float>(HostContext()->GetGUIParamNormalized(paramTopo->runtimeParamIndex));
  auto mode = static_cast<FFGUISettingsVisualsFromEngineMode>(paramTopo->static_.List().NormalizedToPlainFast(normalized));
  if (mode == FFGUISettingsVisualsFromEngineMode::Off)
    return FBGUIRenderType::Basic;
  if (mode == FFGUISettingsVisualsFromEngineMode::On)
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

  _outputAndPatch = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 1, 0 } });
  _outputAndPatch->Add(0, 0, FFMakeOutputGUI(this));
  _outputAndPatch->Add(0, 1, FFMakePatchGUI(this));

  _guiSettingsAndTweak = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 0, 1 } });
  _guiSettingsAndTweak->Add(0, 0, FFMakeGUISettingsGUI(this));
  _guiSettingsAndTweak->Add(0, 1, FFMakeTweakGUI(this));

  _topModules = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 0, 0, 1 } });
  _topModules->Add(0, 0, FFMakeVoiceModuleGUI(this));
  _topModules->Add(0, 1, FFMakeMasterGUI(this));
  _topModules->Add(0, 2, FFMakeSettingsGUI(this));

  _matrix = FFMakeModMatrixGUI(this);
  _globalUni = FFMakeGlobalUniGUI(this, _graphRenderState.get(), &_fixedGraphs);
  _main = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int>(7, 1), std::vector<int> { { 1 } });
  _main->Add(0, 0, _topModules);
  _main->Add(1, 0, FFMakeMixGUI(this));
  _main->Add(2, 0, FFMakeOsciGUI(this));
  _main->Add(3, 0, FFMakeEffectGUI(this));
  _main->Add(4, 0, FFMakeEchoGUI(this));
  _main->Add(5, 0, FFMakeLFOGUI(this));
  _main->Add(6, 0, FFMakeEnvGUI(this, _msegEditors));

  _tabs = StoreComponent<FBAutoSizeTabComponent>();
  _tabs->addTab("Main", Colours::black, _main, false);
  _tabs->addTab("Matrix", Colours::black, _matrix, false);
  _tabs->addTab("Unison", Colours::black, _globalUni, false);

  _container = StoreComponent<FBGridComponent>(false, 0, -1, std::vector<int> { { 6, 6, 9, 92 } }, std::vector<int> { { 1 } });
  _container->Add(0, 0, _outputAndPatch);
  _container->Add(1, 0, _guiSettingsAndTweak);
  _container->Add(2, 0, _headerAndGraph);
  _container->Add(3, 0, _tabs);

  _osciParamListener = std::make_unique<FFOsciParamListener>(this);
  _vMixParamListener = std::make_unique<FFVMixParamListener>(this);
  _gMixParamListener = std::make_unique<FFGMixParamListener>(this);
  _effectParamListener = std::make_unique<FFEffectParamListener>(this);
  _modMatrixParamListener = std::make_unique<FFModMatrixParamListener>(this);
  _globalUniParamListener = std::make_unique<FFGlobalUniParamListener>(this);
  _voiceModuleParamListener = std::make_unique<FFVoiceModuleParamListener>(this);
  _envParamListener = std::make_unique<FFEnvParamListener>(this, _msegEditors);

  addAndMakeVisible(_container);
}