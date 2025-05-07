#include <playground_base/gui/shared/FBGUIConfig.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/shared/FBGUILookAndFeel.hpp>
#include <playground_base/base/shared/FBUserDataFolder.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

#include <filesystem>

using namespace juce;

static float
ConvertExchangeValueFromSkewed(FBStaticParamBase const& param, float normalized)
{
  if (param.type != FBParamType::Linear)
    return normalized;
  NormalisableRange<float> range(0.0f, 1.0f, 0.0f, static_cast<float>(param.Linear().editSkewFactor));
  return range.convertTo0to1(normalized);
}

void 
FBGUILookAndFeel::DrawLinearSliderExchangeThumb(
  Graphics& g, FBParamSlider& slider, 
  int y, int height, float exchangeValue) 
{
  auto layout = getSliderLayout(slider);
  int sliderRegionStart = layout.sliderBounds.getX();
  int sliderRegionSize = layout.sliderBounds.getWidth();
  float skewed = ConvertExchangeValueFromSkewed(slider.Param()->static_, exchangeValue);
  float minExchangePos = static_cast<float>(sliderRegionStart + skewed * sliderRegionSize);
  float kx = minExchangePos;
  float ky = static_cast<float>(y) + height * 0.5f;
  Point<float> maxPoint = { kx, ky };
  float thumbWidth = static_cast<float>(getSliderThumbRadius(slider));
  g.setColour(slider.findColour(Slider::thumbColourId).withAlpha(0.5f));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(maxPoint));
}

void 
FBGUILookAndFeel::DrawRotarySliderExchangeThumb(
  Graphics& g, FBParamSlider& slider,
  int x, int y, int width, int height,
  float rotaryStartAngle, float rotaryEndAngle, 
  float exchangeValue)
{
  auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);
  auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
  float skewed = ConvertExchangeValueFromSkewed(slider.Param()->static_, exchangeValue);
  auto toAngle = rotaryStartAngle + skewed * (rotaryEndAngle - rotaryStartAngle);
  auto lineW = jmin(8.0f, radius * 0.5f);
  auto arcRadius = radius - lineW * 0.5f;
  auto thumbWidth = lineW * 2.0f;
  Point<float> thumbPoint(
    bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
    bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
  g.setColour(slider.findColour(Slider::thumbColourId).withAlpha(0.5f));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

FBGUILookAndFeel::
FBGUILookAndFeel()
{
  File selfJuce(File::getSpecialLocation(File::currentExecutableFile));
  std::filesystem::path selfPath(selfJuce.getFullPathName().toStdString());
  auto fontPath = selfPath.parent_path().parent_path() / "Resources" / "ui" / "font.ttf";
  auto fontBytes = FBReadFile(fontPath);
  _typeface = Typeface::createSystemTypefaceFor(fontBytes.data(), fontBytes.size());
  _font = Font(FontOptions(_typeface)).withHeight(FBGUIFontSize);
}

BorderSize<int> 
FBGUILookAndFeel::getLabelBorderSize(
  Label&)
{
  return { 1, 2, 1, 2 };
}

void 
FBGUILookAndFeel::positionComboBoxText(
  ComboBox& b, Label& l)
{
  l.setFont(getComboBoxFont(b));
  l.setBorderSize(getLabelBorderSize(l));
  l.setBounds(3, 1, b.getWidth() - 6, b.getHeight() - 2);
}

void 
FBGUILookAndFeel::drawLabel(
  Graphics& g, Label& label)
{
  g.fillAll(label.findColour(Label::backgroundColourId));
  auto alpha = label.isEnabled() ? 1.0f : 0.5f;
  const Font font(getLabelFont(label));
  g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
  g.setFont(font);

  auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
  g.drawText(label.getText(), textArea, label.getJustificationType(), false);
  g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
  g.drawRect(label.getLocalBounds());
}

void
FBGUILookAndFeel::drawComboBox(Graphics& g,
  int	width, int height, bool	isButtonDown,
  int	buttonX, int buttonY, int	buttonW, int buttonH, ComboBox& box)
{
  auto cornerSize = 3.0f;
  Rectangle<int> boxBounds(2, 1, width - 4, height - 2);

  g.setColour(box.findColour(ComboBox::backgroundColourId));
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

  g.setColour(box.findColour(ComboBox::outlineColourId));
  g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
}

void
FBGUILookAndFeel::drawLinearSlider(
  Graphics& g,
  int x, int y, int width, int height,
  float sliderPos, float minSliderPos, float maxSliderPos,
  Slider::SliderStyle style, Slider& s)
{
  FBParamSlider* paramSlider;
  LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, s);
  if ((paramSlider = dynamic_cast<FBParamSlider*>(&s)) == nullptr)
    return;
  auto paramActive = paramSlider->ParamActiveExchangeState();
  if (!paramActive.active)
    return;
  DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramActive.minValue);
  if (paramSlider->Param()->static_.IsVoice())
    DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramActive.maxValue);
}

void
FBGUILookAndFeel::drawRotarySlider(
  Graphics& g,
  int x, int y, int width, int height, float sliderPos,
  float rotaryStartAngle, float rotaryEndAngle, Slider& s)
{
  // juce default reduces by 10
  x -= 7;
  y -= 7;
  width += 14;
  height += 14;

  FBParamSlider* paramSlider;
  LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, s);
  if ((paramSlider = dynamic_cast<FBParamSlider*>(&s)) == nullptr)
    return;
  auto paramActive = paramSlider->ParamActiveExchangeState();
  if (!paramActive.active)
    return;
  DrawRotarySliderExchangeThumb(g, *paramSlider, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.minValue);
  if (paramSlider->Param()->static_.IsVoice())
    DrawRotarySliderExchangeThumb(g, *paramSlider, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.maxValue);
}