#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/gui/FFPatchGUI.hpp>
#include <firefly_synth/gui/FFTweakGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/lfo/FFLFOGUI.hpp>
#include <firefly_synth/modules/mix/FFMixGUI.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/echo/FFEchoGUI.hpp>
#include <firefly_synth/modules/other/FFOtherGUI.hpp>
#include <firefly_synth/modules/other/FFOtherTopo.hpp>
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
#include <firefly_base/gui/graph/FBModuleGraphComponent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>

using namespace juce;

FFMainTabChangedListener::
FFMainTabChangedListener(FFPlugGUI* plugGUI):
_plugGUI(plugGUI) {}

void 
FFMainTabChangedListener::changeListenerCallback(ChangeBroadcaster*)
{
  _plugGUI->HideAllOverlaysAndFileBrowsers();
}

FFPlugGUI::
~FFPlugGUI() { }

FFPlugGUI::
FFPlugGUI(FBHostGUIContext* hostContext):
FBPlugGUI(hostContext),
_graphRenderState(std::make_unique<FBGraphRenderState>(this))
{
  FB_LOG_ENTRY_EXIT();
  // needs to be before SetupGUI()
  bool found = false;
  for(int i = 0; i < Themes().size(); i++)
    if (Themes()[i].global.name == HostContext()->ThemeName())
    {
      SwitchTheme(HostContext()->ThemeName());
      found = true;
      break;
    }
  if (!found)
    SwitchTheme(HostContext()->Topo()->static_->defaultThemeName);
  SetupGUI();
  InitAllDependencies();
  resized();
}

void 
FFPlugGUI::ForceReLayout()
{
  // called after the theme changed
  // because of custom font size this may change the size of labels etc very deep in the visual tree
  // without affecting their parent size
  // plain resized() doesnt cut it, so just whack it here

  auto bounds = getLocalBounds();
  getChildComponent(0)->setBounds(0, 0, bounds.getWidth() / 2, bounds.getHeight() / 2);
  getChildComponent(0)->resized();
  getChildComponent(0)->setBounds(bounds);
  getChildComponent(0)->resized();
}

void
FFPlugGUI::BeforePatchChanged()
{
  FBParamTopoIndices indices = { { (int)FFModuleType::Other, 0 }, { (int)FFOtherParam::FlushAudioToggle, 0 } };
  double flushNorm = HostContext()->GetAudioParamNormalized(indices);
  _prevFlushAudioToggle = flushNorm >= 0.5;
}

void
FFPlugGUI::AfterPatchChanged()
{
  // Get old stuff out of the delay lines.
  FBParamTopoIndices indices = { { (int)FFModuleType::Other, 0 }, { (int)FFOtherParam::FlushAudioToggle, 0 } };
  HostContext()->SetAudioParamBool(indices, _prevFlushAudioToggle);
  FlushAudio();

  // Update show tweaked from session/patch/default.
  repaint();
}

void 
FFPlugGUI::OnPatchLoaded()
{
  FBPlugGUI::OnPatchLoaded();
  if (onPatchLoaded)
    onPatchLoaded();
}

void 
FFPlugGUI::OnPatchNameChanged(std::string const& name)
{
  FBPlugGUI::OnPatchNameChanged(name);
  if (onPatchNameChanged)
    onPatchNameChanged(name);
}

void
FFPlugGUI::OnInstanceNameChanged(std::string const& name)
{
  FBPlugGUI::OnInstanceNameChanged(name);
  if (onInstanceNameChanged)
    onInstanceNameChanged(name);
}

void 
FFPlugGUI::RequestFixedGraphsRerender(int moduleIndex)
{
  for (int i = 0; i < _fixedGraphs.size(); i++)
    _fixedGraphs[i]->RequestRerender(
      moduleIndex == -1? _fixedGraphs[i]->FixedToRuntimeModuleIndex(): moduleIndex);
}

void
FFPlugGUI::SwitchDetailsSectionToModule(int index, int slot)
{
  if (index == (int)FFModuleType::VMix)
    _detailContent->SetContent(_vMixDetails);
  if (index == (int)FFModuleType::GMix)
    _detailContent->SetContent(_gMixDetails);
  if (index == (int)FFModuleType::VEcho)
    _detailContent->SetContent(_vEchoDetails);
  if (index == (int)FFModuleType::GEcho)
    _detailContent->SetContent(_gEchoDetails);
  if (index == (int)FFModuleType::Env)
    _detailContent->SetContent(_envDetails[slot]);
  if (index == (int)FFModuleType::Osci)
    _detailContent->SetContent(_osciDetails[slot]);
  if (index == (int)FFModuleType::VLFO)
    _detailContent->SetContent(_vLFODetails[slot]);
  if (index == (int)FFModuleType::GLFO)
    _detailContent->SetContent(_gLFODetails[slot]);
  if (index == (int)FFModuleType::VEffect)
    _detailContent->SetContent(_vEffectDetails[slot]);
  if (index == (int)FFModuleType::GEffect)
    _detailContent->SetContent(_gEffectDetails[slot]);
}

void 
FFPlugGUI::RequestMainGraphsRerender(int index, int slot)
{
  if (index == -1)
  {
    _fxMainGraph->RequestRerender(_fxMainGraph->TweakedModuleByUI());
    _oscMainGraph->RequestRerender(_oscMainGraph->TweakedModuleByUI());
    _envMainGraph->RequestRerender(_envMainGraph->TweakedModuleByUI());
    _lfoMainGraph->RequestRerender(_lfoMainGraph->TweakedModuleByUI());
    _echoMainGraph->RequestRerender(_echoMainGraph->TweakedModuleByUI());
    return;
  }

  if (index == (int)FFModuleType::Env)
    _envMainGraph->RequestRerender(HostContext()->Topo()->moduleTopoToRuntime.at({ index, slot }));
  if (index == (int)FFModuleType::Osci)
    _oscMainGraph->RequestRerender(HostContext()->Topo()->moduleTopoToRuntime.at({ index, slot }));
  if (index == (int)FFModuleType::VEffect || index == (int)FFModuleType::GEffect)
    _fxMainGraph->RequestRerender(HostContext()->Topo()->moduleTopoToRuntime.at({ index, slot }));
  if (index == (int)FFModuleType::VLFO || index == (int)FFModuleType::GLFO)
    _lfoMainGraph->RequestRerender(HostContext()->Topo()->moduleTopoToRuntime.at({ index, slot }));
  if (index == (int)FFModuleType::VEcho || index == (int)FFModuleType::GEcho)
    _echoMainGraph->RequestRerender(HostContext()->Topo()->moduleTopoToRuntime.at({ index, slot }));
}

void
FFPlugGUI::UpdateExchangeStateTick()
{
  FBPlugGUI::UpdateExchangeStateTick();
  RequestFixedGraphsRerender(-1);
  RequestMainGraphsRerender(-1, -1);
  _detailsGraph->RequestRerender(_detailsGraph->TweakedModuleByUI());
}

void
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

void
FFPlugGUI::SwitchGraphsToModule(int index, int slot)
{
  auto topo = HostContext()->Topo()->ModuleAtTopo({ index, slot });
  _detailsGraph->RequestRerender(topo->runtimeModuleIndex);
  RequestMainGraphsRerender(index, slot);
}

void 
FFPlugGUI::ModuleSlotClicked(int index, int slot)
{
  SwitchGraphsToModule(index, slot);
  SwitchDetailsSectionToModule(index, slot);
}

void
FFPlugGUI::ActiveModuleSlotChanged(int index, int slot)
{
  SwitchGraphsToModule(index, slot);
  SwitchDetailsSectionToModule(index, slot);
}

void 
FFPlugGUI::GUIParamNormalizedChanged(int index, double normalized)
{
  FBPlugGUI::GUIParamNormalizedChanged(index, normalized);
  int moduleIndex = HostContext()->Topo()->gui.params[index].runtimeModuleIndex;
  _detailsGraph->RequestRerender(moduleIndex);
  RequestFixedGraphsRerender(moduleIndex);
  auto tweakedIndices = HostContext()->Topo()->gui.params[index].topoIndices.module;
  RequestMainGraphsRerender(tweakedIndices.index, tweakedIndices.slot);

  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0}, {(int)FFGUISettingsGUIParam::HilightTweakMode, 0 } };
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
  _detailsGraph->RequestRerender(moduleIndex);
  RequestFixedGraphsRerender(moduleIndex);
  auto tweakedIndices = HostContext()->Topo()->audio.params[index].topoIndices.module;
  RequestMainGraphsRerender(tweakedIndices.index, tweakedIndices.slot);
}

void
FFPlugGUI::AudioParamNormalizedChangedFromHost(int index, double normalized)
{
  FBPlugGUI::AudioParamNormalizedChangedFromHost(index, normalized);
  if (HostContext()->Topo()->audio.params[index].static_.mode == FBParamMode::Output)
    return;
  auto tweakedIndices = HostContext()->Topo()->audio.params[index].topoIndices.module;
  RequestMainGraphsRerender(tweakedIndices.index, tweakedIndices.slot);
  int tweakedModule = HostContext()->Topo()->audio.params[index].runtimeModuleIndex;
  if (_detailsGraph->TweakedModuleByUI() == tweakedModule)
    _detailsGraph->RequestRerender(_detailsGraph->TweakedModuleByUI());
  RequestFixedGraphsRerender(tweakedModule);
}

FBParamModulationBoundsSource 
FFPlugGUI::GetParamModulationBounds(int index, double& minNorm, double& maxNorm) const
{
  int result = FBParamModulationBoundsSource::None;
  float valueNorm = (float)HostContext()->GetAudioParamNormalized(index);
  float currentMinNorm = valueNorm;
  float currentMaxNorm = valueNorm;    

  // Matrix goes first, as in DSP!
  if (FFModMatrixAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm))
    result |= FBParamModulationBoundsSource::Matrix;

  // Followed by direct access.
  if(FFOsciAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm))
    result |= FBParamModulationBoundsSource::DirectAccess;
  if(FFVMixAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm))
    result |= FBParamModulationBoundsSource::DirectAccess;
  if(FFGMixAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm))
    result |= FBParamModulationBoundsSource::DirectAccess;
  if(FFEffectAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm))
    result |= FBParamModulationBoundsSource::DirectAccess;
  if(FFVoiceModuleAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm))
    result |= FBParamModulationBoundsSource::DirectAccess;

  // Followed by unison.
  if(FFGlobalUniAdjustParamModulationGUIBounds(HostContext(), index, currentMinNorm, currentMaxNorm))
    result |= FBParamModulationBoundsSource::Unison;

  minNorm = result != FBParamModulationBoundsSource::None ? currentMinNorm: 0.0;
  maxNorm = result != FBParamModulationBoundsSource::None ? currentMaxNorm : 0.0f;
  return (FBParamModulationBoundsSource)result;
}

bool
FFPlugGUI::HighlightModulationBounds() const
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightMod, 0 } };
  return HostContext()->GetGUIParamBool(indices);
}

FBHighlightTweakMode
FFPlugGUI::HighlightTweakedMode() const
{
  FBParamTopoIndices indices = { { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightTweakMode, 0 } };
  return HostContext()->GetGUIParamList<FBHighlightTweakMode>(indices);
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
  FBParamTopoIndices indices = { { (int)FFModuleType::Other, 0 }, { (int)FFOtherParam::FlushAudioToggle, 0 } };
  double flushNorm = HostContext()->GetAudioParamNormalized(indices);
  double newFlushNorm = flushNorm > 0.5 ? 0.0 : 1.0;
  HostContext()->PerformImmediateAudioParamEdit(indices, newFlushNorm);
}

void 
FFPlugGUI::SetupGUI()
{
  FB_LOG_ENTRY_EXIT();

  _detailsGraph = StoreComponent<FBModuleGraphComponent>(this, true, _graphRenderState.get(), -1, -1, [this]() { return GetRenderType(true); });
  _fxMainGraph = StoreComponent<FBModuleGraphComponent>(this, false, _graphRenderState.get(), -1, -1, [this]() { return GetRenderType(true); });
  _oscMainGraph = StoreComponent<FBModuleGraphComponent>(this, false, _graphRenderState.get(), -1, -1, [this]() { return GetRenderType(true); });
  _envMainGraph = StoreComponent<FBModuleGraphComponent>(this, false, _graphRenderState.get(), -1, -1, [this]() { return GetRenderType(true); });
  _lfoMainGraph = StoreComponent<FBModuleGraphComponent>(this, false, _graphRenderState.get(), -1, -1, [this]() { return GetRenderType(true); });
  _echoMainGraph = StoreComponent<FBModuleGraphComponent>(this, false, _graphRenderState.get(), -1, -1, [this]() { return GetRenderType(true); });
  _mainAndDetailGraphs = StoreComponent<FBGridComponent>(this, true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 1, 1, 1, 1, 1 } });
  _mainAndDetailGraphs->Add(0, 0, StoreComponent<FBThemedComponent>(this, (int)FFThemedComponentId::MainGraphs, _oscMainGraph));
  _mainAndDetailGraphs->Add(0, 1, StoreComponent<FBThemedComponent>(this, (int)FFThemedComponentId::MainGraphs, _fxMainGraph));
  _mainAndDetailGraphs->Add(0, 2, StoreComponent<FBThemedComponent>(this, (int)FFThemedComponentId::MainGraphs, _echoMainGraph));
  _mainAndDetailGraphs->Add(0, 3, StoreComponent<FBThemedComponent>(this, (int)FFThemedComponentId::MainGraphs, _lfoMainGraph));
  _mainAndDetailGraphs->Add(0, 4, StoreComponent<FBThemedComponent>(this, (int)FFThemedComponentId::MainGraphs, _envMainGraph));

  _outputOtherAndPatch = StoreComponent<FBGridComponent>(this, false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 1, 0, 0 } });
  _outputOtherAndPatch->Add(0, 0, FFMakeOutputGUI(this));
  _outputOtherAndPatch->Add(0, 1, FFMakeOtherGUI(this));
  _outputOtherAndPatch->Add(0, 2, FFMakePatchGUI(this));

  _guiSettingsAndTweak = StoreComponent<FBGridComponent>(this, false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { { 2, 1 } });
  _guiSettingsAndTweak->Add(0, 0, FFMakeGUISettingsGUI(this));
  _guiSettingsAndTweak->Add(0, 1, FFMakeTweakGUI(this));

  _matrix = FFMakeModMatrixGUI(this);
  _detailContent = StoreComponent<FBContentComponent>();
  _globalUni = FFMakeGlobalUniGUI(this, _graphRenderState.get(), &_fixedGraphs);
  _vMixDetails = FFMakeVMixDetailGUI(this);
  _gMixDetails = FFMakeGMixDetailGUI(this);
  _gEchoDetails = FFMakeEchoDetailGUI(this, true);
  _vEchoDetails = FFMakeEchoDetailGUI(this, false);
  for (int i = 0; i < FFEnvCount; i++)
    _envDetails[i] = FFMakeEnvDetailGUI(this, i);
  for (int i = 0; i < FFOsciCount; i++)
    _osciDetails[i] = FFMakeOsciDetailGUI(this, i);
  for (int i = 0; i < FFLFOCount; i++)
  {
    _gLFODetails[i] = FFMakeLFODetailGUI(this, true, i);
    _vLFODetails[i] = FFMakeLFODetailGUI(this, false, i);
  }
  for (int i = 0; i < FFEffectCount; i++)
  {
    _gEffectDetails[i] = FFMakeEffectDetailGUI(this, true, i);
    _vEffectDetails[i] = FFMakeEffectDetailGUI(this, false, i);
  }

  _voiceMaster = StoreComponent<FBGridComponent>(this, false, -1, -1, std::vector<int> { 1 }, std::vector<int> { { 1, 0 } });
  _voiceMaster->Add(0, 0, FFMakeVoiceModuleGUI(this));
  _voiceMaster->Add(0, 1, FFMakeMasterGUI(this));
  _main = StoreComponent<FBGridComponent>(this, false, -1, -1, std::vector<int> { 2, 2, 2, 2, 2, 2, 1 }, std::vector<int> { { 21, 10, 9 } });
  _main->Add(0, 0, _voiceMaster);
  _main->Add(0, 2, FFMakeSettingsGUI(this));
  _main->Add(1, 0, FFMakeMixGUI(this));
  _main->Add(2, 0, FFMakeOsciGUI(this));
  _main->Add(3, 0, FFMakeEffectGUI(this));
  _main->Add(4, 0, FFMakeEchoGUI(this));
  _main->Add(5, 0, FFMakeLFOGUI(this));
  _main->Add(6, 0, FFMakeEnvGUI(this, _msegEditors));
  _main->Add(0, 1, 3, 1, StoreComponent<FBMarginComponent>(this, true, true, false, true, 
      StoreComponent<FBThemedComponent>(this, (int)FFThemedComponentId::DetailGraphs, _detailsGraph)));
  _main->Add(3, 1, 4, 1, _detailContent);

  _tabs = StoreComponent<FBAutoSizeTabComponent>(this, true);
  _tabs->addTab("MAIN", Colours::black, StoreComponent<FBMarginComponent>(this, false, false, true, false, _main), false);
  _tabs->addTab("MATRIX", Colours::black, StoreComponent<FBMarginComponent>(this, false, false, true, false, _matrix), false);
  _tabs->addTab("UNISON", Colours::black, StoreComponent<FBMarginComponent>(this, false, false, true, false, _globalUni), false);
  _mainTabChangedListener = std::make_unique<FFMainTabChangedListener>(this);
  _tabs->getTabbedButtonBar().addChangeListener(_mainTabChangedListener.get());

  _container = StoreComponent<FBGridComponent>(this, false, 0, -1, std::vector<int> { { 6, 6, 24, 92 } }, std::vector<int> { { 1 } });
  _container->Add(0, 0, _outputOtherAndPatch);
  _container->Add(1, 0, _guiSettingsAndTweak);
  _container->Add(2, 0, _mainAndDetailGraphs);
  _container->Add(3, 0, StoreComponent<FBThemedComponent>(this, (int)FFThemedComponentId::MainSelector, _tabs));

  _osciParamListener = std::make_unique<FFOsciParamListener>(this);
  _vMixParamListener = std::make_unique<FFVMixParamListener>(this);
  _gMixParamListener = std::make_unique<FFGMixParamListener>(this);
  _effectParamListener = std::make_unique<FFEffectParamListener>(this);
  _modMatrixParamListener = std::make_unique<FFModMatrixParamListener>(this);
  _globalUniParamListener = std::make_unique<FFGlobalUniParamListener>(this);
  _voiceModuleParamListener = std::make_unique<FFVoiceModuleParamListener>(this);
  _envParamListener = std::make_unique<FFEnvParamListener>(this, _msegEditors);

  _containerMargin = StoreComponent<FBMarginComponent>(this, true, true, true, true, _container);
  addAndMakeVisible(_containerMargin);
}