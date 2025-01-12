#pragma once

#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>

struct FBRuntimeTopo;
class IFBHostGUIContext;

class FFPlugGUI final:
public FBPlugGUI
{
  float _scale = 1.0f;
  FBRuntimeTopo const* const _topo;
  IFBHostGUIContext* const _hostContext;
  std::vector<std::unique_ptr<juce::Component>> _labels = {};
  std::vector<std::unique_ptr<juce::Component>> _controls = {};

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(
    FBRuntimeTopo const* topo, 
    IFBHostGUIContext* hostContext);

  int MinWidth() const override { return 200; }
  int MaxWidth() const override { return 1200; }
  int DefaultWidth() const override { return 600; }
  int AspectRatioWidth() const override { return 1; }
  int AspectRatioHeight() const override { return 1; }

  void resized() override;
  void SetContentScaleFactor(float scale) override;
  void SetParamNormalized(int index, float normalized) override;
};