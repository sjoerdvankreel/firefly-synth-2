#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBContentComponent.hpp>

#include <memory>
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
  juce::Component* _overlayComponent = {};
  juce::Component* _overlayContainer = {};
  FBContentComponent* _overlayContent = {};

  FBModuleGraphComponent* _graph = {};
  std::unique_ptr<FBGraphRenderState> _graphRenderState;

  void SetupGUI();
  FBGUIRenderType GetRenderType(int paramIndex) const;

protected:
  void OnPatchChanged() override;
  void UpdateExchangeStateTick() override;

public:
  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  void FlushDelayLines();
  void ToggleMatrix(bool on);
  void SwitchGraphToModule(int index, int slot);
  
  void HideOverlayComponent();
  void ShowOverlayComponent(juce::Component* overlay, int w, int h);

  void resized() override;
  FBGUIRenderType GetKnobRenderType() const override;
  FBGUIRenderType GetGraphRenderType() const override;
  void ModuleSlotClicked(int index, int slot) override;
  void ActiveModuleSlotChanged(int index, int slot) override;
  void GUIParamNormalizedChanged(int index, double normalized) override;
  void AudioParamNormalizedChangedFromUI(int index, double normalized) override;
  void AudioParamNormalizedChangedFromHost(int index, double normalized) override;
};