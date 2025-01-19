#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamSlider final:
public juce::Slider,
public FBParamControl
{
  IFBHostGUIContext* const _context;

public:
  FBParamSlider(
    FBRuntimeParam const* param, juce::Component* root, 
    IFBHostGUIContext* context, juce::Slider::SliderStyle style);

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
  
  void SetValueNormalized(float normalized) override;
  juce::String getTextFromValue(double value) override;
  double getValueFromText(const juce::String& text) override;
};