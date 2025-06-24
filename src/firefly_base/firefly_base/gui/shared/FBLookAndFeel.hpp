#pragma once

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBParamSlider;

class FBLookAndFeel:
public juce::LookAndFeel_V4
{
  void DrawLinearSliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider, 
    int y, int height, float exchangeValue);

  void DrawRotarySliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider,
    int x, int y, int width, int height,
    float rotaryStartAngle, float rotaryEndAngle, 
    float exchangeValue);

public:  
  juce::BorderSize<int> getLabelBorderSize(
    juce::Label&) override;

  void positionComboBoxText(
    juce::ComboBox&, juce::Label&) override;

  int getTabButtonBestWidth(
    juce::TabBarButton& button, int tabDepth) override;

  void drawLabel(
    juce::Graphics&, juce::Label&) override;

  void drawTabButton(
    juce::TabBarButton& button, juce::Graphics& g,
    bool isMouseOver, bool isMouseDown) override;

  void drawTickBox(
    juce::Graphics& g, juce::Component& component,
    float x, float y, float w, float h,
    const bool ticked,
    const bool isEnabled,
    const bool shouldDrawButtonAsHighlighted,
    const bool shouldDrawButtonAsDown) override;

  void drawComboBox(juce::Graphics&,
    int	width, int height, bool	isButtonDown,
    int	buttonX, int buttonY, int	buttonW, int buttonH,
    juce::ComboBox&) override;

  void drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, 
    bool shouldDrawButtonAsDown) override;

  void drawLinearSlider(
    juce::Graphics&,
    int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    juce::Slider::SliderStyle, juce::Slider&) override;

  void drawRotarySlider(
    juce::Graphics&,
    int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

  juce::Font getPopupMenuFont() override { return FBGUIGetFont(); }
  juce::Font getLabelFont(juce::Label&) override { return FBGUIGetFont(); }
  juce::Font getComboBoxFont(juce::ComboBox&) override { return FBGUIGetFont(); }
  juce::Font getSliderPopupFont(juce::Slider&) override { return FBGUIGetFont(); }
  juce::Font getTextButtonFont(juce::TextButton&, int)  override { return FBGUIGetFont(); }
};