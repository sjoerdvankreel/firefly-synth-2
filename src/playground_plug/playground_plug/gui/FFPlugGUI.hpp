#pragma once

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBGraphProcState.hpp>

struct FBRuntimeTopo;
class FBHostGUIContext;
class FBModuleGraphComponent;

class FFPlugGUI final:
public FBPlugGUI
{
  FBGraphProcState _graphProcState;
  FBModuleGraphComponent* _graph = {};

  void SetupGUI();
  void RequestGraphRender(int paramIndex);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBRuntimeTopo const* topo, FBHostGUIContext* hostContext);

  void resized() override;
  void ParamNormalizedChangedFromUI(int index) override;
  void SetParamNormalizedFromHost(int index, float normalized) override;
};