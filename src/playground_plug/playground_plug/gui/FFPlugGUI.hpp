#pragma once

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <chrono>
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
  std::chrono::high_resolution_clock::time_point _exchangeUpdated = {};

  void SetupGUI();
  void RequestGraphRender(int paramIndex);

public:
  ~FFPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBHostGUIContext* hostContext);

  void resized() override;

  void UpdateExchangeState() override;
  void SetParamNormalizedFromUI(int index, float normalized) override;
  void SetParamNormalizedFromHost(int index, float normalized) override;
};