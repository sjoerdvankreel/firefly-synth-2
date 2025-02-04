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
  float _minExchangeValue = 0.0f;
  float _maxExchangeValue = 0.0f;
  bool _exchangeValueReceived = false;

public:
  FBParamSlider(
    FBPlugGUI* plugGUI, 
    FBRuntimeParam const* param, 
    juce::Slider::SliderStyle style);

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
  void mouseUp(juce::MouseEvent const& event) override;
  
  void UpdateExchangeState();
  int FixedWidth(int height) const override;
  juce::String getTextFromValue(double value) override;
  void SetValueNormalizedFromHost(float normalized) override;
  double getValueFromText(const juce::String& text) override;

  float MinExchangeValue() const { return _minExchangeValue; }
  float MaxExchangeValue() const { return _maxExchangeValue; }
  bool ExchangeValueReceived() const { return _exchangeValueReceived; }
};