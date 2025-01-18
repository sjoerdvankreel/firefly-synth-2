#pragma once

#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FBRuntimeTopo;
class IFBHostGUIContext;

class FFPlugGUI final:
public FBPlugGUI
{
public:
  void resized() override;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBRuntimeTopo const* topo, IFBHostGUIContext* hostContext);  
};