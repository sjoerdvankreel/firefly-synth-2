#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>

#include <memory>

struct FBRuntimeTopo;
class IFBHostGUIContext;

class FFPlugGUI final:
public FBPlugGUI
{
  FBRuntimeTopo const* const _topo;
  IFBHostGUIContext* const _hostContext;
  std::unique_ptr<FBParamSlider> _slider = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(
    FBRuntimeTopo const* topo, 
    IFBHostGUIContext* hostContext);

  int MinWidth() const override { return 200; }
  int MaxWidth() const override { return 1200; }
  int DefaultWidth() const override { return 600; }
  int AspectRatioWidth() const override { return 4; }
  int AspectRatioHeight() const override { return 3; }

  void resized() override;
  void SetParamNormalized(int index, float normalized) override;
};