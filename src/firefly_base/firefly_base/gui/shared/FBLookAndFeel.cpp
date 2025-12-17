#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

static const int TabSizeSmall = 40;
static const int TabSizeLarge = 60;

// Juce delegates combo tooltip to label tooltip, but we never set that one.
// We just generate tooltips on the fly. So instead, have the label ask the combo for the tip.
class FBComboBoxLabel :
public juce::Label
{
  ComboBox* _box;

public:
  String getTooltip() override;
  FBComboBoxLabel(ComboBox* box) : _box(box) {}
};

String
FBComboBoxLabel::getTooltip()
{
  FBParamComboBox* paramBox = nullptr;
  if ((paramBox = dynamic_cast<FBParamComboBox*>(_box)) != nullptr)
    return paramBox->FBParamComboBox::getTooltip();
  FBGUIParamComboBox* guiParamBox = nullptr;
  if ((guiParamBox = dynamic_cast<FBGUIParamComboBox*>(_box)) != nullptr)
    return guiParamBox->FBGUIParamComboBox::getTooltip();
  return Label::getTooltip();
}

static double
ConvertValueFromSkewed(FBStaticParamBase const& param, double normalized)
{
  if (param.type != FBParamType::Linear)
    return normalized;
  NormalisableRange<double> range(0.0, 1.0, 0.0, param.Linear().editSkewFactor);
  return range.convertTo0to1(normalized);
}

static Colour
GetSliderThumbColor(Slider const& s)
{
  bool isHighlightTweaked = false;
  FBParamSlider const* ps = dynamic_cast<FBParamSlider const*>(&s);
  if (ps != nullptr)
    isHighlightTweaked = ps->IsHighlightTweaked();
  auto result = isHighlightTweaked? Colours::white : s.findColour(Slider::thumbColourId);
  return s.isEnabled() ? result : result.darker(0.6f);
}

static bool
GetSliderModulationBounds(Slider const& s, double& minNorm, double& maxNorm)
{
  if (!s.isEnabled())
    return false;
  FBParamSlider const* ps = dynamic_cast<FBParamSlider const*>(&s);
  if (ps == nullptr)
    return false;
  if (!ps->PlugGUI()->HighlightModulationBounds())
    return false;
  if (ps->PlugGUI()->GetParamModulationBounds(ps->Param()->runtimeParamIndex, minNorm, maxNorm) == FBParamModulationBoundsSource::None)
    return false;
  auto const& staticParam = ps->Param()->static_;
  minNorm = ConvertValueFromSkewed(staticParam, minNorm);
  maxNorm = ConvertValueFromSkewed(staticParam, maxNorm);
  return true;
}

static void CreateTabTextLayout(
  const TabBarButton& button, 
  float length, Colour colour, 
  Font const& font, bool centerText, 
  std::string text, TextLayout& textLayout)
{
  AttributedString s;
  if (dynamic_cast<FBTabBarButton const*>(&button))
  {
    if (!centerText)
      text = " " + text;
    s.setJustification(centerText ? Justification::centred : Justification::centredLeft);
  }
  s.append(text, font, colour);
  textLayout.createLayout(s, length);
}

FBColorScheme const& 
FBLookAndFeel::FindColorSchemeFor(
  Component const& c) const
{
  static FBColorScheme fallback = {};
  
  if (auto p = dynamic_cast<FBParamControl const*>(&c))
  {
    int rtModuleIndex = p->Param()->runtimeModuleIndex;
    auto moduleIter = Theme().moduleColors.find(rtModuleIndex);
    if (moduleIter != Theme().moduleColors.end())
    {
      int rtParamIndex = p->Param()->runtimeParamIndex;
      auto paramIter = moduleIter->second.audioParamColorSchemes.find(rtParamIndex);
      if (paramIter == moduleIter->second.audioParamColorSchemes.end())
        return Theme().colorSchemes.at(moduleIter->second.colorScheme);
      return Theme().colorSchemes.at(paramIter->second);
    }
    return Theme().defaultColorScheme;
  }
   
  if (auto p = dynamic_cast<FBGUIParamControl const*>(&c))
  {
    int rtModuleIndex = p->Param()->runtimeModuleIndex;
    auto moduleIter = Theme().moduleColors.find(rtModuleIndex);
    if (moduleIter != Theme().moduleColors.end())
    {
      int rtParamIndex = p->Param()->runtimeParamIndex;
      auto paramIter = moduleIter->second.guiParamColorSchemes.find(rtParamIndex);
      if (paramIter == moduleIter->second.guiParamColorSchemes.end())
        return Theme().colorSchemes.at(moduleIter->second.colorScheme);
      return Theme().colorSchemes.at(paramIter->second);
    }
    return Theme().defaultColorScheme;
  }

  if(auto gui = c.findParentComponentOfClass<FBPlugGUI>())
    if (auto m = c.findParentComponentOfClass<FBModuleComponent>())
    {
      int rtModuleIndex = gui->HostContext()->Topo()->moduleTopoToRuntime.at({ m->ModuleIndex(), m->ModuleSlot() });
      auto moduleIter = Theme().moduleColors.find(rtModuleIndex);
      if (moduleIter != Theme().moduleColors.end())
        return Theme().colorSchemes.at(moduleIter->second.colorScheme);
      return Theme().defaultColorScheme;
    }

  return fallback;
}

void 
FBLookAndFeel::DrawLinearSliderExchangeThumb(
  Graphics& g, FBParamSlider& slider, 
  int y, int height, float exchangeValue) 
{
  auto layout = getSliderLayout(slider);
  int sliderRegionStart = layout.sliderBounds.getX();
  int sliderRegionSize = layout.sliderBounds.getWidth();
  float skewed = (float)ConvertValueFromSkewed(slider.Param()->static_, exchangeValue);
  float minExchangePos = static_cast<float>(sliderRegionStart + skewed * sliderRegionSize);
  float kx = minExchangePos;
  float ky = static_cast<float>(y) + height * 0.5f;
  Point<float> maxPoint = { kx, ky };
  float thumbWidth = static_cast<float>(getSliderThumbRadius(slider));
  g.setColour(GetSliderThumbColor(slider).withAlpha(0.5f));
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
  float skewed = (float)ConvertValueFromSkewed(slider.Param()->static_, exchangeValue);
  auto toAngle = rotaryStartAngle + skewed * (rotaryEndAngle - rotaryStartAngle);
  auto lineW = jmin(8.0f, radius * 0.5f);
  auto arcRadius = radius - lineW * 0.5f;
  auto thumbWidth = lineW * 2.0f;
  Point<float> thumbPoint(
    bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
    bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
  g.setColour(GetSliderThumbColor(slider).withAlpha(0.5f));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

void
FBLookAndFeel::DrawTabButtonPart(
  TabBarButton& button, Graphics& g,
  bool isMouseOver, bool isMouseDown,
  bool toggleState, bool centerText,
  std::string const& text,
  Rectangle<int> const& activeArea)
{
  const Colour bkg(button.getTabBackgroundColour());

  if (toggleState)
    g.setColour(bkg);
  else
    g.setGradientFill(ColourGradient(
      bkg.brighter(0.2f), activeArea.getTopLeft().toFloat(),
      bkg.darker(0.1f), activeArea.getBottomLeft().toFloat(), false));

  g.fillRect(activeArea);
  g.setColour(button.findColour(TabbedButtonBar::tabOutlineColourId));

  Rectangle<int> r(activeArea);
  g.fillRect(r.removeFromTop(1));
  g.fillRect(r.removeFromLeft(1));
  g.fillRect(r.removeFromRight(1));
  g.fillRect(r.removeFromBottom(1));

  const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;
  Colour col(bkg.contrasting().withMultipliedAlpha(alpha));

  if (TabbedButtonBar* bar = button.findParentComponentOfClass<TabbedButtonBar>())
  {
    TabbedButtonBar::ColourIds colID = button.isFrontTab() ? TabbedButtonBar::frontTextColourId : TabbedButtonBar::tabTextColourId;
    if (bar->isColourSpecified(colID))
      col = bar->findColour(colID);
    else if (isColourSpecified(colID))
      col = findColour(colID);
  }

  TextLayout textLayout;
  const Rectangle<float> area(activeArea.toFloat());
  float length = area.getWidth();
  float depth = area.getHeight();
  ::CreateTabTextLayout(button, length, col, FBGUIGetFont(), centerText, text, textLayout);

  g.addTransform(AffineTransform::translation(area.getX(), area.getY()));
  textLayout.draw(g, Rectangle<float>(length, depth));
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
  int barWidth = button.getTabbedButtonBar().getWidth();
  int numTabs = button.getTabbedButtonBar().getNumTabs();
  if (numTabs == 1)
    return barWidth;
  FBModuleTabBarButton* fbButton = dynamic_cast<FBModuleTabBarButton*>(&button);
  if (fbButton == nullptr)
  {
    int tabWidth = barWidth / numTabs;
    int remain = barWidth - tabWidth * numTabs;    
    if (button.getTabbedButtonBar().indexOfTabButton(&button) == numTabs - 1)
      tabWidth += remain;
    return tabWidth;
  }
  bool large = fbButton->large;
  bool hasSeparatorText = !fbButton->GetSeparatorText().empty();
  int result = large ? TabSizeLarge : TabSizeSmall;
  if (hasSeparatorText)
    result += TabSizeLarge;
  return result;
}

Label* 
FBLookAndFeel::createComboBoxTextBox(
  ComboBox& box)
{
  return new FBComboBoxLabel(&box);
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
  if (dynamic_cast<FBAutoSizeLabel2*>(&label) ||
      dynamic_cast<FBLastTweakedLabel*>(&label))
  {
    g.setColour(Colour(0xFF333333));
    g.fillRoundedRectangle(label.getLocalBounds().toFloat(), 2.0f);
  }

  auto const& scheme = FindColorSchemeFor(label);
  auto foreground = scheme.foreground;
  if (auto b = label.findParentComponentOfClass<ComboBox>())
    foreground = scheme.controlForeground.darker(b->isEnabled() ? 0.0f : scheme.dimDisabled);

  g.setFont(getLabelFont(label));
  g.setColour(foreground);
  auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
  g.drawText(label.getText(), textArea, label.getJustificationType(), false);
}

void
FBLookAndFeel::drawToggleButton(
  Graphics& g, ToggleButton& button,
  bool shouldDrawButtonAsHighlighted,
  bool shouldDrawButtonAsDown)
{
  auto fontSize = jmin(15.0f, (float)button.getHeight() * 0.75f);
  auto tickWidth = fontSize * 1.1f;
  float x = (button.getWidth() - tickWidth) * 0.5f;
  drawTickBox(g, button, x, ((float)button.getHeight() - tickWidth) * 0.5f,
    tickWidth, tickWidth,
    button.getToggleState(),
    button.isEnabled(),
    shouldDrawButtonAsHighlighted,
    shouldDrawButtonAsDown);
}

void
FBLookAndFeel::drawComboBox(Graphics& g,
  int	width, int height, bool	/*isButtonDown*/,
  int	/*buttonX*/, int /*buttonY*/, int	/*buttonW*/, int /*buttonH*/, ComboBox& box)
{
  auto cornerSize = 2.0f;
  auto const& scheme = FindColorSchemeFor(box);
  Rectangle<int> boxBounds(2, 2, width - 4, height - 4);

  Colour background = scheme.controlBackground;
  auto* paramCombo = dynamic_cast<FBParamComboBox*>(&box);
  if (paramCombo != nullptr && paramCombo->IsHighlightTweaked())
    background = scheme.controlTweaked;
  g.setColour(background);
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

  g.setColour(scheme.controlBorder);
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
  auto const& scheme = FindColorSchemeFor(component);

  Colour background = scheme.controlBackground;
  auto* paramToggle = dynamic_cast<FBParamToggleButton*>(&component);
  if (paramToggle != nullptr && paramToggle->IsHighlightTweaked())
    background = scheme.controlTweaked;
  g.setColour(background);
  g.fillRoundedRectangle(tickBounds, 2.0f);

  g.setColour(scheme.controlBorder);
  g.drawRoundedRectangle(tickBounds, 2.0f, 1.0f);

  if (ticked)
  {
    auto pathBounds = tickBounds.reduced(6.0f, 6.0f);
    g.setColour(scheme.controlForeground.darker(component.isEnabled()? 0.0f: scheme.dimDisabled));
    Path p;
    p.startNewSubPath(pathBounds.getTopLeft());
    p.lineTo(pathBounds.getBottomRight());
    g.strokePath(p, PathStrokeType(3.0f, PathStrokeType::JointStyle::beveled, PathStrokeType::EndCapStyle::rounded));
    p = {};
    p.startNewSubPath(pathBounds.getBottomLeft());
    p.lineTo(pathBounds.getTopRight());
    g.strokePath(p, PathStrokeType(3.0f, PathStrokeType::JointStyle::beveled, PathStrokeType::EndCapStyle::rounded));
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

  FB_ASSERT(slider.isHorizontal());
  Path backgroundTrackFull;
  Point<float> startPointFull((float)x, (float)y + (float)height * 0.5f);
  Point<float> endPointFull((float)(width + x), startPointFull.y);
  backgroundTrackFull.startNewSubPath(startPointFull);
  backgroundTrackFull.lineTo(endPointFull);
  g.setColour(slider.findColour(Slider::backgroundColourId));
  g.strokePath(backgroundTrackFull, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

  Path valueTrack;
  Point<float> minPoint, maxPoint, thumbPoint;
  auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
  auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
  minPoint = startPointFull;
  maxPoint = { kx, ky };
  auto thumbWidth = getSliderThumbRadius(slider);

  valueTrack.startNewSubPath(minPoint);
  valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
  g.setColour(slider.findColour(Slider::trackColourId));
  g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

  double modMin, modMax;
  FBParamSlider* paramSlider = dynamic_cast<FBParamSlider*>(&slider);
  if (GetSliderModulationBounds(slider, modMin, modMax))
  {
    Path backgroundTrackMod;
    Point<float> startPointMod((float)(x + width * modMin), (float)y + (float)height * 0.5f);
    Point<float> endPointMod((float)(width * modMax + x), startPointMod.y);
    backgroundTrackMod.startNewSubPath(startPointMod);
    backgroundTrackMod.lineTo(endPointMod);
    g.setColour(Colours::white.withAlpha(0.5f));
    g.strokePath(backgroundTrackMod, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
  }

  g.setColour(GetSliderThumbColor(slider));
  g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint));

  if (paramSlider == nullptr)
    return;
  auto paramActive = paramSlider->ParamActiveExchangeState();
  if (!paramActive.active)
    return;
  DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramActive.minValue);
  if (paramSlider->Param()->static_.IsVoice())
    DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramActive.maxValue);
}

void 
FBLookAndFeel::drawButtonBackground(
  juce::Graphics& g, juce::Button& button, 
  const juce::Colour& backgroundColour,
  bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
  auto cornerSize = 6.0f;
  auto bounds = button.getLocalBounds().toFloat().reduced(1.0f, 1.0f);

  auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
    .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

  if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
    baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

  g.setColour(baseColour);

  auto flatOnLeft = button.isConnectedOnLeft();
  auto flatOnRight = button.isConnectedOnRight();
  auto flatOnTop = button.isConnectedOnTop();
  auto flatOnBottom = button.isConnectedOnBottom();

  if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
  {
    Path path;
    path.addRoundedRectangle(bounds.getX(), bounds.getY(),
      bounds.getWidth(), bounds.getHeight(),
      cornerSize, cornerSize,
      !(flatOnLeft || flatOnTop),
      !(flatOnRight || flatOnTop),
      !(flatOnLeft || flatOnBottom),
      !(flatOnRight || flatOnBottom));

    g.fillPath(path);

    g.setColour(button.findColour(ComboBox::outlineColourId));
    g.strokePath(path, PathStrokeType(1.0f));
  }
  else
  {
    g.fillRoundedRectangle(bounds, cornerSize);

    g.setColour(button.findColour(ComboBox::outlineColourId));
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
  }
}

void 
FBLookAndFeel::drawButtonText(
  Graphics& g, TextButton& button,
  bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
{
  Font font(getTextButtonFont(button, button.getHeight()));
  g.setFont(font);
  g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
    : TextButton::textColourOffId)
    .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

  const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
  const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

  const int fontHeight = roundToInt(font.getHeight() * 0.6f);
  const int leftIndent = jmin(fontHeight, 1 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
  const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
  const int textWidth = button.getWidth() - leftIndent - rightIndent;

  if (textWidth > 0)
    g.drawText(button.getButtonText(),
      leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
      Justification::centred, false);
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

  double minNorm;
  double maxNorm;
  if (GetSliderModulationBounds(slider, minNorm, maxNorm))
  {
    Path modArc;
    auto minAngle = rotaryStartAngle + minNorm * (rotaryEndAngle - rotaryStartAngle);
    auto maxAngle = rotaryStartAngle + maxNorm * (rotaryEndAngle - rotaryStartAngle);
    modArc.addCentredArc(
      bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
      0.0f, (float)minAngle, (float)maxAngle, true);
    g.setColour(Colours::white.withAlpha(0.5f));
    g.strokePath(modArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
  }

  auto thumbWidth = lineW * 2.0f;
  Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
    bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
  g.setColour(GetSliderThumbColor(slider));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));

  FBParamSlider* paramSlider = dynamic_cast<FBParamSlider*>(&slider);
  if (paramSlider == nullptr)
    return;
  auto paramActive = paramSlider->ParamActiveExchangeState();
  if (!paramActive.active)
    return;
  DrawRotarySliderExchangeThumb(g, *paramSlider, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.minValue);
  if (paramSlider->Param()->static_.IsVoice())
    DrawRotarySliderExchangeThumb(g, *paramSlider, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.maxValue);
}

juce::Rectangle<int> 
FBLookAndFeel::getTooltipBounds(
  const juce::String& tipText,
  juce::Point<int> screenPos,
  juce::Rectangle<int> parentArea)
{
  auto trimmed = tipText.trim();

  float th = 0.0f;
  float tw = 0.0f;
  float fontSize = 13.0f;
  float textHeight = FBGUIGetFontHeightFloat() + 2.0f;
  auto lines = FBStringSplit(trimmed.toStdString(), "\r\n");
  auto font = Font(FontOptions(fontSize, Font::bold).withMetricsKind(getDefaultMetricsKind()));
  for (int i = 0; i < lines.size(); i++)
  {
    th += textHeight;
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    // The JUCE suggested alternatives hang on occasion.
    tw = std::max(tw, font.getStringWidthFloat(lines[i]));
#if _MSC_VER
#pragma warning(pop)
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
  }

  int pad = 3;
  int itw = (int)std::ceil(tw) + 2 * pad + 2;
  int ith = (int)std::ceil(th) + 2 * pad + 2;
  return Rectangle<int>(screenPos.x > parentArea.getCentreX() ? screenPos.x - (itw + 12) : screenPos.x + 24,
    screenPos.y > parentArea.getCentreY() ? screenPos.y - (ith + 6) : screenPos.y + 6,
    itw, ith)
    .constrainedWithin(parentArea);
}

void 
FBLookAndFeel::drawTooltip(
  Graphics& g, const String& text,
  int width, int height)
{
  auto cornerSize = 5.0f;
  Rectangle<int> bounds(width, height);
  g.setColour(findColour(TooltipWindow::backgroundColourId));
  g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
  g.setColour(findColour(TooltipWindow::outlineColourId));
  g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

  float pad = 3.0f;
  float fontSize = 13.0f;
  auto trimmed = text.trim();
  float textHeight = FBGUIGetFontHeightFloat() + 2.0f;
  auto lines = FBStringSplit(trimmed.toStdString(), "\r\n");
  auto textBounds = Rectangle<float>(pad, pad, width - 2.0f * pad, textHeight);
  g.setColour(findColour(TooltipWindow::textColourId));
  g.setFont(Font(FontOptions(fontSize, Font::bold).withMetricsKind(getDefaultMetricsKind())));
  while (lines.size() > 0)
  {
    if (lines[0].size() > 0)
      g.drawText(lines[0], textBounds, Justification::left, false);
    lines.erase(lines.begin());
    textBounds.translate(0.0f, textHeight);
  }
  g.setColour(Colours::white);
  g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 2.0f);
}

void
FBLookAndFeel::drawTabButton(
  TabBarButton& button, Graphics& g,
  bool isMouseOver, bool isMouseDown)
{
  bool large = {};
  bool centerText = {};
  std::string separatorText = {};
  FBModuleTabBarButton* fbButton = dynamic_cast<FBModuleTabBarButton*>(&button);
  if (fbButton != nullptr)
  {
    large = fbButton->large;
    centerText = fbButton->centerText;
    separatorText = fbButton->GetSeparatorText();
  }

  bool toggleState = button.getToggleState();
  isMouseOver &= button.getTabbedButtonBar().getNumTabs() > 1;
  isMouseDown &= button.getTabbedButtonBar().getNumTabs() > 1;
  toggleState &= button.getTabbedButtonBar().getNumTabs() > 1;

  Rectangle<int> activeArea(
    button.getActiveArea().getX(), button.getActiveArea().getY(),
    button.getActiveArea().getWidth(), button.getActiveArea().getHeight());
  std::string buttonText = button.getButtonText().toStdString();

  if (separatorText.empty())
  {
    DrawTabButtonPart(button, g, isMouseOver, isMouseDown, toggleState, centerText, buttonText, activeArea);
    return;
  }

  int size = large ? TabSizeLarge : TabSizeSmall;
  auto separatorArea = Rectangle<int>(activeArea.getX(), activeArea.getY(), TabSizeLarge - 1, activeArea.getHeight());
  DrawTabButtonPart(button, g, false, false, false, false, separatorText, separatorArea);
  auto buttonArea = Rectangle<int>(activeArea.getX() + TabSizeLarge, activeArea.getY(), size, activeArea.getHeight());
  DrawTabButtonPart(button, g, isMouseOver, isMouseDown, toggleState, centerText, buttonText, buttonArea);
}