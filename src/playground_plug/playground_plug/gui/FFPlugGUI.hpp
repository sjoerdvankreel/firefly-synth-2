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
  void SetupGUI();
  FBGraphProcState _graphProcState;
  FBModuleGraphComponent* _graph = {};

public:
  void resized() override;
  void ParamNormalizedChangedFromUI(int index) override;

  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBRuntimeTopo const* topo, FBHostGUIContext* hostContext);
};