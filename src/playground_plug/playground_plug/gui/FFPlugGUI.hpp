#pragma once

#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

class FFPlugGUI final:
public FBPlugGUI
{
  juce::Slider _s;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugGUI);
  FFPlugGUI(IFBHostGUIContext* hostContext);

  int MinWidth() const override { return 200; }
  int MaxWidth() const override { return 1200; }
  int DefaultWidth() const override { return 600; }
  int AspectRatioWidth() const override { return 4; }
  int AspectRatioHeight() const override { return 3; }

  void resized() override;
  void SetParamNormalized(int index, float normalized) override;
};