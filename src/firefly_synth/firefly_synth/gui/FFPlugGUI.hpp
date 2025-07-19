#pragma once

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <memory>
#include <unordered_map>

class FBGridComponent;
class FBHostGUIContext;
class FBGraphRenderState;
class FBModuleGraphComponent;

class FFPlugGUI final:
public FBPlugGUI
{
  FBGridComponent* _content = {};
  juce::Component* _voiceMatrix = {};
  juce::Component* _globalMatrix = {};
  FBModuleGraphComponent* _graph = {};
  std::unique_ptr<FBGraphRenderState> _graphRenderState;

  void SetupGUI();

public:
  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  void ShowVoiceMatrix();
  void ShowGlobalMatrix();

  void resized() override;
  void UpdateExchangeStateTick() override;
  FBGUIRenderType GetRenderType() const override;
  void ActiveModuleSlotChanged(int index, int slot) override;
  void GUIParamNormalizedChanged(int index, double normalized) override;
  void AudioParamNormalizedChangedFromUI(int index, double normalized) override;
  void AudioParamNormalizedChangedFromHost(int index, double normalized) override;
};