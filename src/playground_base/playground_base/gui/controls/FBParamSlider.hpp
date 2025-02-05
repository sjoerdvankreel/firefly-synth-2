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
  bool _exchangeActive = false;
  float _minExchangeValue = 0.0f;
  float _maxExchangeValue = 0.0f;

public:
  FBParamSlider(
    FBPlugGUI* plugGUI, 
    FBRuntimeParam const* param, 
    juce::Slider::SliderStyle style);

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

  bool ExchangeActive() const { return _exchangeActive; }
  float MinExchangeValue() const { return _minExchangeValue; }
  float MaxExchangeValue() const { return _maxExchangeValue; }
};