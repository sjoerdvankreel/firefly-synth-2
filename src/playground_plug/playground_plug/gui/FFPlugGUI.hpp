#pragma once

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FBRuntimeTopo;
class FBHostGUIContext;

class FFPlugGUI final:
public FBPlugGUI
{
public:
  void resized() override;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBRuntimeTopo const* topo, FBHostGUIContext* hostContext);
};