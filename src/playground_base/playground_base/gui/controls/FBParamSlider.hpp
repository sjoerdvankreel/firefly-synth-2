#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamSlider final:
public juce::Slider,
public FBParamControl,
public IFBHorizontalAutoSize
{
public:
  FBParamSlider(
    FBPlugGUI* plugGUI, 
    FBRuntimeParam const* param, 
    juce::Slider::SliderStyle style);

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
  
  int FixedWidth(int height) const override;
  void SetValueNormalized(float normalized) override;
  juce::String getTextFromValue(double value) override;
  double getValueFromText(const juce::String& text) override;
};