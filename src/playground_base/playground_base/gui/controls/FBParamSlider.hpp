#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <playground_base/base/state/FBExchangeParamActiveState.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamSlider final:
public juce::Slider,
public FBParamControl,
public IFBHorizontalAutoSize
{
  FBExchangeParamActiveState _paramActive = {};

public:
  FBParamSlider(
    FBPlugGUI* plugGUI, 
    FBRuntimeParam const* param, 
    juce::Slider::SliderStyle style);

  FBExchangeParamActiveState const&
  ExchangeParamActiveState() const { return _paramActive; }

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
  void mouseUp(juce::MouseEvent const& event) override;
  
  juce::String getTooltip() override;
  juce::String getTextFromValue(double value) override;
  double getValueFromText(const juce::String& text) override;

  void UpdateExchangeState();
  int FixedWidth(int height) const override;
  void SetValueNormalizedFromHost(float normalized) override;
};