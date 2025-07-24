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
  bool _showMatrix = false;
  juce::Component* _matrix = {};
  FBGridComponent* _modules = {};
  FBGridComponent* _container = {};
  FBContentComponent* _content = {};

  FBModuleGraphComponent* _graph = {};
  std::unique_ptr<FBGraphRenderState> _graphRenderState;

  void SetupGUI();

public:
  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  bool ToggleMatrix();
  void SwitchGraphToModule(int index, int slot);

  void resized() override;
  void UpdateExchangeStateTick() override;
  FBGUIRenderType GetRenderType() const override;
  void ActiveModuleSlotChanged(int index, int slot) override;
  void GUIParamNormalizedChanged(int index, double normalized) override;
  void AudioParamNormalizedChangedFromUI(int index, double normalized) override;
  void AudioParamNormalizedChangedFromHost(int index, double normalized) override;
};