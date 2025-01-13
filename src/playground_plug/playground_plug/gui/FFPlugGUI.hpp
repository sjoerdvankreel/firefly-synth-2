#pragma once

#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>

struct FBRuntimeTopo;
class IFBHostGUIContext;

class FFPlugGUI final:
public FBPlugGUI
{
  FBRuntimeTopo const* const _topo;
  IFBHostGUIContext* const _hostContext;
  std::vector<std::unique_ptr<juce::Component>> _labels = {};
  std::vector<std::unique_ptr<juce::Component>> _controls = {};

protected:
  IFBParamControl* 
  GetParamControlForIndex(int index) override;

public:
  void resized() override;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(FBRuntimeTopo const* topo, IFBHostGUIContext* hostContext);  
};