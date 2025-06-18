#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

static Colour
getSliderThumbColor(Slider const& s)
{
  auto result = s.findColour(Slider::thumbColourId);
  return s.isEnabled() ? result : result.darker(0.6f);
}

static void createTabTextLayout(
  const TabBarButton& button, float length, 
  Colour colour, Font const& font,
  TextLayout& textLayout)
{
  AttributedString s;
  String text = button.getButtonText();
  if (auto* fbButton = dynamic_cast<FBTabBarButton const*>(&button))
  {
    if (!fbButton->centerText)
      text = " " + text;
    s.setJustification(fbButton->centerText ? Justification::centred : Justification::centredLeft);
  }
  s.append(button.getButtonText(), font, colour);
  textLayout.createLayout(s, length);
}

static float
ConvertExchangeValueFromSkewed(FBStaticParamBase const& param, float normalized)
{
  if (param.type != FBParamType::Linear)
    return normalized;
  NormalisableRange<float> range(0.0f, 1.0f, 0.0f, static_cast<float>(param.Linear().editSkewFactor));
  return range.convertTo0to1(normalized);
}

void 
FBLookAndFeel::DrawLinearSliderExchangeThumb(
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
  g.setColour(getSliderThumbColor(slider).withAlpha(0.5f));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(maxPoint));
}

void 
FBLookAndFeel::DrawRotarySliderExchangeThumb(
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
  g.setColour(getSliderThumbColor(slider).withAlpha(0.5f));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

BorderSize<int> 
FBLookAndFeel::getLabelBorderSize(
  Label&)
{
  return { 1, 2, 1, 2 };
}

int
FBLookAndFeel::getTabButtonBestWidth(
  juce::TabBarButton& button, int /*tabDepth*/)
{
  auto text = button.getButtonText().toStdString();
  return text.size() == 1 ? 32 : 64;
}

void 
FBLookAndFeel::positionComboBoxText(
  ComboBox& b, Label& l)
{
  l.setFont(getComboBoxFont(b));
  l.setBorderSize(getLabelBorderSize(l));
  l.setBounds(3, 1, b.getWidth() - 6, b.getHeight() - 2);
}

void 
FBLookAndFeel::drawLabel(
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
FBLookAndFeel::drawComboBox(Graphics& g,
  int	width, int height, bool	/*isButtonDown*/,
  int	/*buttonX*/, int /*buttonY*/, int	/*buttonW*/, int /*buttonH*/, ComboBox& box)
{
  auto cornerSize = 3.0f;
  Rectangle<int> boxBounds(2, 1, width - 4, height - 2);

  g.setColour(box.findColour(ComboBox::backgroundColourId));
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

  g.setColour(box.findColour(ComboBox::outlineColourId));
  g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
}

void 
FBLookAndFeel::drawTickBox(
  Graphics& g, Component& component,
  float x, float y, float w, float h,
  const bool ticked,
  const bool /*isEnabled*/,
  const bool /*shouldDrawButtonAsHighlighted*/,
  const bool /*shouldDrawButtonAsDown*/)
{
  Rectangle<float> tickBounds(x, y, w, h);
  g.setColour(component.findColour(ToggleButton::tickDisabledColourId));
  g.drawRoundedRectangle(tickBounds, 4.0f, 1.0f);

  if (ticked)
  {
    auto color = component.findColour(ToggleButton::tickColourId);
    if (!component.isEnabled())
      color = color.darker(0.6f);
    g.setColour(color);
    auto tick = getTickShape(0.75f);
    g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(4, 5).toFloat(), false));
  }
}

void
FBLookAndFeel::drawLinearSlider(
  Graphics& g,
  int x, int y, int width, int height,
  float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
  Slider::SliderStyle style, Slider& slider)
{
  auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);
  auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

  Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
   slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));
  Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
    slider.isHorizontal() ? startPoint.y : (float)y);

  Path backgroundTrack;
  backgroundTrack.startNewSubPath(startPoint);
  backgroundTrack.lineTo(endPoint);
  g.setColour(slider.findColour(Slider::backgroundColourId));
  g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

  Path valueTrack;
  Point<float> minPoint, maxPoint, thumbPoint;
  auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
  auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
  minPoint = startPoint;
  maxPoint = { kx, ky };
  auto thumbWidth = getSliderThumbRadius(slider);

  valueTrack.startNewSubPath(minPoint);
  valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
  g.setColour(slider.findColour(Slider::trackColourId));
  g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
  g.setColour(getSliderThumbColor(slider));
  g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint));

  FBParamSlider* paramSlider;
  if ((paramSlider = dynamic_cast<FBParamSlider*>(&slider)) == nullptr)
    return;
  auto paramActive = paramSlider->ParamActiveExchangeState();
  if (!paramActive.active)
    return;
  DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramActive.minValue);
  if (paramSlider->Param()->static_.IsVoice())
    DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramActive.maxValue);
}

void
FBLookAndFeel::drawRotarySlider(
  Graphics& g,
  int x, int y, int width, int height, float sliderPos,
  float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
{
  // juce default reduces by 10
  x -= 7;
  y -= 7;
  width += 14;
  height += 14;

  auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
  auto fill = slider.findColour(Slider::rotarySliderFillColourId);
  auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);
  auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
  auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  auto lineW = jmin(8.0f, radius * 0.5f);
  auto arcRadius = radius - lineW * 0.5f;

  Path backgroundArc;
  backgroundArc.addCentredArc(
    bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
    0.0f, rotaryStartAngle, rotaryEndAngle, true);
  g.setColour(outline);
  g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

  if (slider.isEnabled())
  {
    Path valueArc;
    valueArc.addCentredArc(
      bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
      0.0f, rotaryStartAngle, toAngle, true);
    g.setColour(fill);
    g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
  }

  auto thumbWidth = lineW * 2.0f;
  Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
    bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
  g.setColour(getSliderThumbColor(slider));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));

  FBParamSlider* paramSlider;
  if ((paramSlider = dynamic_cast<FBParamSlider*>(&slider)) == nullptr)
    return;
  auto paramActive = paramSlider->ParamActiveExchangeState();
  if (!paramActive.active)
    return;
  DrawRotarySliderExchangeThumb(g, *paramSlider, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.minValue);
  if (paramSlider->Param()->static_.IsVoice())
    DrawRotarySliderExchangeThumb(g, *paramSlider, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.maxValue);
}

void
FBLookAndFeel::drawTabButton(
  TabBarButton& button, Graphics& g,
  bool isMouseOver, bool isMouseDown)
{
  const Rectangle<int> activeArea(button.getActiveArea());
  const TabbedButtonBar::Orientation o = button.getTabbedButtonBar().getOrientation();
  const Colour bkg(button.getTabBackgroundColour());

  if (button.getToggleState())
  {
    g.setColour(bkg);
  }
  else
  {
    Point<int> p1, p2;
    switch (o)
    {
    case TabbedButtonBar::TabsAtBottom:   
      p1 = activeArea.getBottomLeft(); 
      p2 = activeArea.getTopLeft();    
      break;
    case TabbedButtonBar::TabsAtTop:      
      p1 = activeArea.getTopLeft();    
      p2 = activeArea.getBottomLeft(); 
      break;
    case TabbedButtonBar::TabsAtRight:    
      p1 = activeArea.getTopRight();   
      p2 = activeArea.getTopLeft();    
      break;
    case TabbedButtonBar::TabsAtLeft:     
      p1 = activeArea.getTopLeft();    
      p2 = activeArea.getTopRight();   
      break;
    default:                              
      jassertfalse; break;
    }

    g.setGradientFill(ColourGradient(
      bkg.brighter(0.2f), p1.toFloat(),
      bkg.darker(0.1f), p2.toFloat(), false));
  }

  g.fillRect(activeArea);
  g.setColour(button.findColour(TabbedButtonBar::tabOutlineColourId));

  Rectangle<int> r(activeArea);
  if (o != TabbedButtonBar::TabsAtBottom)  
    g.fillRect(r.removeFromTop(1));
  if (o != TabbedButtonBar::TabsAtTop)     
    g.fillRect(r.removeFromBottom(1));
  if (o != TabbedButtonBar::TabsAtRight)   
    g.fillRect(r.removeFromLeft(1));
  if (o != TabbedButtonBar::TabsAtLeft)    
    g.fillRect(r.removeFromRight(1));

  const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;
  Colour col(bkg.contrasting().withMultipliedAlpha(alpha));

  if (TabbedButtonBar* bar = button.findParentComponentOfClass<TabbedButtonBar>())
  {
    TabbedButtonBar::ColourIds colID = button.isFrontTab() ? TabbedButtonBar::frontTextColourId: TabbedButtonBar::tabTextColourId;
    if (bar->isColourSpecified(colID))
      col = bar->findColour(colID);
    else if (isColourSpecified(colID))
      col = findColour(colID);
  }

  const Rectangle<float> area(button.getTextArea().toFloat());
  float length = area.getWidth();
  float depth = area.getHeight();
  if (button.getTabbedButtonBar().isVertical())
    std::swap(length, depth);

  TextLayout textLayout;
  ::createTabTextLayout(button, length, col, FBGUIGetFont(), textLayout);

  AffineTransform t;
  switch (o)
  {
  case TabbedButtonBar::TabsAtLeft:  
    t = t.rotated(MathConstants<float>::pi * -0.5f).translated(area.getX(), area.getBottom()); 
    break;
  case TabbedButtonBar::TabsAtRight: 
    t = t.rotated(MathConstants<float>::pi * 0.5f).translated(area.getRight(), area.getY()); 
    break;
  case TabbedButtonBar::TabsAtTop:
  case TabbedButtonBar::TabsAtBottom:
    t = t.translated(area.getX(), area.getY()); 
    break;
  default:             
    jassertfalse; 
    break;
  }

  g.addTransform(t);
  textLayout.draw(g, Rectangle<float>(length, depth));
}