#pragma once

#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBContentComponent.hpp>

#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

class FBHostGUIContext;
class FBGraphRenderState;
class FBModuleGraphComponent;

class FFPlugGUI final:
public FBPlugGUI
{
  juce::Component* _matrix = {};
  FBGridComponent* _modules = {};
  FBGridComponent* _container = {};
  FBGridComponent* _topModules = {};
  FBGridComponent* _headerAndGraph = {};
  FBGridComponent* _outputGUIAndPatch = {};

  FBContentComponent* _content = {};
  juce::Label* _overlayCaption = {};
  std::function<void()> _overlayInit = {};
  juce::Component* _overlayComponent = {};
  juce::Component* _overlayContainer = {};
  FBContentComponent* _overlayContent = {};

  FBModuleGraphComponent* _mainGraph = {};
  std::vector<FBModuleGraphComponent*> _fixedGraphs = {};
  std::unique_ptr<FFGlobalUniParamListener> _globalUniParamListener = {};
  
  std::unique_ptr<FBGraphRenderState> _graphRenderState;

  void SetupGUI();
  FBGUIRenderType GetRenderType() const;

protected:
  void OnPatchChanged() override;
  void UpdateExchangeStateTick() override;

public:
  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  void FlushAudio();
  void ToggleMatrix(bool on);
  void RequestFixedGraphsRerender(int moduleIndex);
  void SwitchMainGraphToModule(int index, int slot);
  
  void HideOverlayComponent();
  void ShowOverlayComponent(
    std::string const& title, 
    juce::Component* overlay, 
    int w, int h, bool vCenter,
    std::function<void()> init);

  FBGUIRenderType GetKnobRenderType() const override { return GetRenderType(); }
  FBGUIRenderType GetGraphRenderType() const override { return GetRenderType(); }

  void resized() override;
  void ModuleSlotClicked(int index, int slot) override;
  void ActiveModuleSlotChanged(int index, int slot) override;
  void GUIParamNormalizedChanged(int index, double normalized) override;
  void AudioParamNormalizedChangedFromUI(int index, double normalized) override;
  void AudioParamNormalizedChangedFromHost(int index, double normalized) override;
};