#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;
class IFBHostGUIContext;

class FBParamSlider final:
public juce::Slider,
public FBParamControl,
public IFBHorizontalAutoSize
{
  IFBHostGUIContext* const _context;

public:
  FBParamSlider(
    FBRuntimeParam const* param, juce::Component* root, 
    IFBHostGUIContext* context, juce::Slider::SliderStyle style);

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
  
  int FixedWidth(int height) const override;
  void SetValueNormalized(float normalized) override;
  juce::String getTextFromValue(double value) override;
  double getValueFromText(const juce::String& text) override;
};