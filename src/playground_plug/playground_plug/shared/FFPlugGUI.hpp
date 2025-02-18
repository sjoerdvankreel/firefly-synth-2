#pragma once

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>

class FBGridComponent;
class FBHostGUIContext;
class FBGraphRenderState;
class FBModuleGraphComponent;

class FFPlugGUI final:
public FBPlugGUI
{
  FBGridComponent* _content = {};
  FBModuleGraphComponent* _graph = {};
  std::unique_ptr<FBGraphRenderState> _graphRenderState;

  void SetupGUI();
  void RequestGraphRender(int moduleIndex);

public:
  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  void resized() override;
  void UpdateExchangeStateTick() override;
  void ActiveModuleSlotChanged(int index, int slot) override;
  void GUIParamNormalizedChanged(int index, float normalized) override;
  void AudioParamNormalizedChangedFromUI(int index, float normalized) override;
  void AudioParamNormalizedChangedFromHost(int index, float normalized) override;
};