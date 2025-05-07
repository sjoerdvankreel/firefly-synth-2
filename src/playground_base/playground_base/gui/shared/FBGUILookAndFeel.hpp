#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBParamSlider;

class FBGUILookAndFeel:
public juce::LookAndFeel_V4
{
  juce::Typeface::Ptr _typeface = {};
  juce::Font _font = {};

  void DrawLinearSliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider, 
    int y, int height, float exchangeValue);

  void DrawRotarySliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider,
    int x, int y, int width, int height,
    float rotaryStartAngle, float rotaryEndAngle, 
    float exchangeValue);

public:  
  
  FBGUILookAndFeel();

  juce::Font getPopupMenuFont() override { return _font; }
  juce::Font getLabelFont(juce::Label&) override { return _font; }
  juce::Font getComboBoxFont(juce::ComboBox&) override { return _font; }
  juce::Font getSliderPopupFont(juce::Slider&) override { return _font; }
  juce::Font getTextButtonFont(juce::TextButton&, int)  override { return _font; }
  juce::Font getTabButtonFont(juce::TabBarButton& b, float) override { return _font; }

  juce::BorderSize<int> getLabelBorderSize(
    juce::Label&) override;

  void positionComboBoxText(
    juce::ComboBox&, juce::Label&) override;

  void drawLabel(
    juce::Graphics&, juce::Label&) override;

  void drawComboBox(juce::Graphics&,
    int	width, int height, bool	isButtonDown,
    int	buttonX, int buttonY, int	buttonW, int buttonH,
    juce::ComboBox&) override;

  void drawLinearSlider(
    juce::Graphics&,
    int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    juce::Slider::SliderStyle, juce::Slider&) override;

  void drawRotarySlider(
    juce::Graphics&,
    int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
};