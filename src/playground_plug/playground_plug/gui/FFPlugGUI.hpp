#pragma once

#include <playground_base/gui/glue/FBPlugGUIBase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>

struct FBRuntimeTopo;
class IFBHostGUIContext;

class FFPlugGUI final:
public FBPlugGUIBase
{
  FBRuntimeTopo const* const _topo;
  IFBHostGUIContext* const _hostContext;
  std::vector<std::unique_ptr<juce::Component>> _labels = {};
  std::vector<std::unique_ptr<juce::Component>> _controls = {};

protected:
  IFBParamControl* 
  GetParamControlForIndex(int index) override;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(
    FBRuntimeTopo const* topo, 
    IFBHostGUIContext* hostContext);

  void resized() override;

  int GetAspectRatioWidth() const override { return 1; }
  int GetAspectRatioHeight() const override { return 1; }
  int GetMinUnscaledWidth() const override { return 200; }
  int GetMaxUnscaledWidth() const override { return 1200; }
  int GetDefaultUnscaledWidth() const override { return 600; }  
};