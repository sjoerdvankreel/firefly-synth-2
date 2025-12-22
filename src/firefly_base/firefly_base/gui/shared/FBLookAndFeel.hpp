#pragma once

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBTheme.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBParamSlider;

class FBLookAndFeel:
public juce::LookAndFeel_V4
{
  FBTheme _theme = {};

  void DrawLinearSliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider, FBColorScheme const& scheme,
    float thumbW, float thumbH, float thumbY, float exchangeValue);

  void DrawRotarySliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider, FBColorScheme const& scheme,
    int x, int y, int width, int height,
    float rotaryStartAngle, float rotaryEndAngle, 
    float exchangeValue);

  void DrawTabButtonPart(
    juce::TabBarButton& button, juce::Graphics& g,
    bool isMouseOver, bool isMouseDown,
    bool toggleState, bool centerText,
    bool isSeparator, std::string const& text,
    juce::Rectangle<int> const& activeArea);

public:  

  FBColorScheme const& FindColorSchemeFor(
    juce::Component const& c) const;

  FBTheme const& Theme() const { return _theme; }
  void SetTheme(FBTheme const& theme) { _theme = FBTheme(theme); }

  juce::BorderSize<int> getLabelBorderSize(
    juce::Label&) override;

  juce::Label* createComboBoxTextBox(
    juce::ComboBox&) override;

  void positionComboBoxText(
    juce::ComboBox&, juce::Label&) override;

  int getTabButtonBestWidth(
    juce::TabBarButton& button, int tabDepth) override;

  void drawLabel(
    juce::Graphics&, juce::Label&) override;

  void fillTextEditorBackground(
    juce::Graphics& g, int width, int height, juce::TextEditor& te) override;

  void drawTextEditorOutline(
    juce::Graphics& g, int width, int height, juce::TextEditor& te) override;

  void drawPopupMenuBackgroundWithOptions(
    juce::Graphics& g, 
    int width, int height, 
    const juce::PopupMenu::Options& options) override;

  void drawPopupMenuItemWithOptions(
    juce::Graphics& g, const juce::Rectangle<int>& area,
    bool isHighlighted,
    const juce::PopupMenu::Item& item,
    const juce::PopupMenu::Options& options) override;

  void drawPopupMenuItem(
    juce::Graphics& g, const juce::Rectangle<int>& area,
    const bool isSeparator, const bool isActive,
    const bool isHighlighted, const bool isTicked,
    const bool hasSubMenu, const juce::String& text,
    const juce::String& shortcutKeyText,
    const juce::Drawable* icon, const juce::Colour* const textColourToUse) override;

  juce::Rectangle<int> getTooltipBounds(
    const juce::String& tipText, 
    juce::Point<int> screenPos, 
    juce::Rectangle<int> parentArea) override;

  void drawTooltip(
    juce::Graphics& g, const juce::String& text,
    int width, int height) override;

  void drawTabbedButtonBarBackground(
    juce::TabbedButtonBar& b, juce::Graphics&) override;

  void drawTabButton(
    juce::TabBarButton& button, juce::Graphics& g,
    bool isMouseOver, bool isMouseDown) override;

  void drawTabAreaBehindFrontButton(
    juce::TabbedButtonBar&, juce::Graphics&,
    const int, const int) override {}

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

  void drawButtonText(juce::Graphics& g, juce::TextButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

  void drawButtonBackground(
    juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

  juce::Font getPopupMenuFont() override { return FBGUIGetFont(); }
  juce::Font getLabelFont(juce::Label&) override { return FBGUIGetFont(); }
  juce::Font getComboBoxFont(juce::ComboBox&) override { return FBGUIGetFont(); }
  juce::Font getSliderPopupFont(juce::Slider&) override { return FBGUIGetFont(); }
  juce::Font getTextButtonFont(juce::TextButton&, int)  override { return FBGUIGetFont(); }
};