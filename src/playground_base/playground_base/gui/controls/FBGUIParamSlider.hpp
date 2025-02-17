#pragma once

#include <playground_base/gui/shared/FBGUIParamControl.hpp>
#include <playground_base/gui/controls/FBAutoSizeSlider.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeGUIParam;

class FBGUIParamSlider final:
public FBAutoSizeSlider,
public FBGUIParamControl
{
public:
  FBGUIParamSlider(
    FBPlugGUI* plugGUI, 
    FBRuntimeGUIParam const* param, 
    juce::Slider::SliderStyle style);

  void valueChanged() override;
  juce::String getTooltip() override;
  juce::String getTextFromValue(double value) override;
  double getValueFromText(const juce::String& text) override;
  void SetValueNormalizedFromPlug(float normalized) override;
};