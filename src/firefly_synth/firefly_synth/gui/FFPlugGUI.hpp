#pragma once

#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/mix/FFVMixGUI.hpp>
#include <firefly_synth/modules/mix/FFGMixGUI.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/effect/FFEffectGUI.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGUI.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBContentComponent.hpp>

#include <array>
#include <memory>
#include <vector>

class FFPlugGUI;
class FBMSEGEditor;
class FBHostGUIContext;
class FBGraphRenderState;
class FBModuleGraphComponent;
class FBAutoSizeTabComponent;

class FFMainTabChangedListener :
public juce::ChangeListener
{
  FFPlugGUI* const _plugGUI;

public:
  FFMainTabChangedListener(FFPlugGUI* plugGUI);
  void changeListenerCallback(juce::ChangeBroadcaster* source) override;
};

class FFPlugGUI final:
public FBPlugGUI
{
  bool _prevFlushAudioToggle = {};
  std::unique_ptr<FBGraphRenderState> _graphRenderState;

  FBGridComponent* _main = {};
  juce::Component* _matrix = {};
  juce::Component* _globalUni = {};
  FBGridComponent* _container = {};
  FBGridComponent* _topModules = {};
  FBAutoSizeTabComponent* _tabs = {};
  FBMarginComponent* _containerMargin = {};
  FBGridComponent* _mainAndDetailGraphs = {};
  FBGridComponent* _outputOtherAndPatch = {};
  FBGridComponent* _guiSettingsAndTweak = {};

  FBModuleGraphComponent* _detailsGraph = {};
  FBModuleGraphComponent* _detailsGraph2 = {}; // TODO
  FBModuleGraphComponent* _fxMainGraph = {};
  FBModuleGraphComponent* _oscMainGraph = {};
  FBModuleGraphComponent* _envMainGraph = {};
  FBModuleGraphComponent* _lfoMainGraph = {};
  FBModuleGraphComponent* _echoMainGraph = {};

  FBContentComponent* _detailContent = {};
  juce::Component* _vEchoDetails = {};
  juce::Component* _gEchoDetails = {};
  std::array<juce::Component*, FFEnvCount> _envDetails = {};
  std::array<juce::Component*, FFLFOCount> _vLFODetails = {};
  std::array<juce::Component*, FFLFOCount> _gLFODetails = {};
  std::array<juce::Component*, FFOsciCount> _osciDetails = {};
  std::array<juce::Component*, FFEffectCount> _vEffectDetails = {};
  std::array<juce::Component*, FFEffectCount> _gEffectDetails = {};

  std::vector<FBMSEGEditor*> _msegEditors = {};
  std::vector<FBModuleGraphComponent*> _fixedGraphs = {};
  std::unique_ptr<FFEnvParamListener> _envParamListener = {};
  std::unique_ptr<FFOsciParamListener> _osciParamListener = {};
  std::unique_ptr<FFVMixParamListener> _vMixParamListener = {};
  std::unique_ptr<FFGMixParamListener> _gMixParamListener = {};
  std::unique_ptr<FFEffectParamListener> _effectParamListener = {};
  std::unique_ptr<FFModMatrixParamListener> _modMatrixParamListener = {};
  std::unique_ptr<FFGlobalUniParamListener> _globalUniParamListener = {};
  std::unique_ptr<FFVoiceModuleParamListener> _voiceModuleParamListener = {};
  std::unique_ptr<FFMainTabChangedListener> _mainTabChangedListener = {};

  void SetupGUI();
  bool HighlightModulationBounds() const override;
  FBHighlightTweakMode HighlightTweakedMode() const override;
  FBGUIRenderType GetRenderType(bool graphOrKnob) const override;
  FBParamModulationBoundsSource GetParamModulationBounds(int index, double& minNorm, double& maxNorm) const override;

protected:
  void ForceReLayout() override;
  void AfterPatchChanged() override;
  void BeforePatchChanged() override;
  void UpdateExchangeStateTick() override;

public:
  std::function<void()> onPatchLoaded = {};
  std::function<void(std::string const&)> onPatchNameChanged = {};
  std::function<void(std::string const&)> onInstanceNameChanged = {};

  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  void FlushAudio();
  void SwitchGraphsToModule(int index, int slot);
  void RequestFixedGraphsRerender(int moduleIndex);
  void RequestMainGraphsRerender(int index, int slot);
  void SwitchDetailsSectionToModule(int index, int slot);

  void resized() override;
  void OnPatchLoaded() override;
  void OnPatchNameChanged(std::string const& name) override;
  void OnInstanceNameChanged(std::string const& name) override;

  void ModuleSlotClicked(int index, int slot) override;
  void ActiveModuleSlotChanged(int index, int slot) override;
  void GUIParamNormalizedChanged(int index, double normalized) override;
  void AudioParamNormalizedChangedFromUI(int index, double normalized) override;
  void AudioParamNormalizedChangedFromHost(int index, double normalized) override;
};