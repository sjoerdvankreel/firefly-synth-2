#pragma once

#include <playground_base/gui/shared/FBAutoSize.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;
struct FBRuntimeGUIParam;

class FBAutoSizeSlider:
public juce::Slider,
public IFBHorizontalAutoSize
{
protected:
  FBAutoSizeSlider(FBPlugGUI* plugGUI, juce::Slider::SliderStyle style);
public:
  int FixedWidth(int height) const override;
};

class FBGUIParamSlider final :
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
  void parentHierarchyChanged() override;
  juce::String getTextFromValue(double value) override;
  double getValueFromText(const juce::String& text) override;
  void SetValueNormalizedFromPlug(double normalized) override;
};

class FBParamSlider final:
public FBAutoSizeSlider,
public FBParamControl
{
  FBParamActiveExchangeState _paramActive = {};

public:
  FBParamSlider(
    FBPlugGUI* plugGUI, 
    FBRuntimeParam const* param, 
    juce::Slider::SliderStyle style);

  FBParamActiveExchangeState const&
  ParamActiveExchangeState() const { return _paramActive; }

  void valueChanged() override;
  void stoppedDragging() override;
  void startedDragging() override;
  void mouseUp(juce::MouseEvent const& event) override;
  
  juce::String getTooltip() override;
  void parentHierarchyChanged() override;
  juce::String getTextFromValue(double value) override;
  double getValueFromText(const juce::String& text) override;

  void UpdateExchangeState();
  void SetValueNormalizedFromHost(double normalized) override;
};