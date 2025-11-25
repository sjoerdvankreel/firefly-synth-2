#pragma once

#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/mix/FFVMixGUI.hpp>
#include <firefly_synth/modules/mix/FFGMixGUI.hpp>
#include <firefly_synth/modules/osci/FFOsciGUI.hpp>
#include <firefly_synth/modules/effect/FFEffectGUI.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixGUI.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleGUI.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>

#include <memory>
#include <vector>

class FBMSEGEditor;
class FBHostGUIContext;
class FBGraphRenderState;
class FBModuleGraphComponent;
class FBAutoSizeTabComponent;

class FFPlugGUI final:
public FBPlugGUI
{
  std::unique_ptr<FBGraphRenderState> _graphRenderState;

  FBGridComponent* _main = {};
  juce::Component* _matrix = {};
  juce::Component* _globalUni = {};
  FBGridComponent* _container = {};
  FBGridComponent* _topModules = {};
  FBAutoSizeTabComponent* _tabs = {};
  FBGridComponent* _headerAndGraph = {};
  FBGridComponent* _outputAndPatch = {};
  FBGridComponent* _guiSettingsAndTweak = {};

  FBModuleGraphComponent* _mainGraph = {};
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

  void SetupGUI();
  bool HighlightModulationBounds() const override;
  FBHighlightTweakMode HighlightTweakedMode() const override;
  FBGUIRenderType GetRenderType(bool graphOrKnob) const override;
  bool GetParamModulationBounds(int index, double& minNorm, double& maxNorm) const override;

protected:
  void OnPatchChanged() override;
  void UpdateExchangeStateTick() override;

public:
  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  void FlushAudio();
  void RequestFixedGraphsRerender(int moduleIndex);
  void SwitchMainGraphToModule(int index, int slot);

  void resized() override;
  void ModuleSlotClicked(int index, int slot) override;
  void ActiveModuleSlotChanged(int index, int slot) override;
  void GUIParamNormalizedChanged(int index, double normalized) override;
  void AudioParamNormalizedChangedFromUI(int index, double normalized) override;
  void AudioParamNormalizedChangedFromHost(int index, double normalized) override;
};