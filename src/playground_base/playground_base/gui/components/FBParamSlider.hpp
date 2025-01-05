#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamSlider final:
public juce::Slider
{
  IFBHostGUIContext* const _context;
  FBRuntimeParam const* const _param;

public:
  FBParamSlider(
    IFBHostGUIContext* context,
    FBRuntimeParam const* param,
    juce::Slider::SliderStyle style);

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
};