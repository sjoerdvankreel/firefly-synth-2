#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamSlider final:
public juce::Slider
{
  FBRuntimeParam const* const _param;
  IFBHostGUIContext* const _context;

public:
  FBParamSlider(
    FBRuntimeParam const* param,
    IFBHostGUIContext* context,
    juce::Slider::SliderStyle style);

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
};