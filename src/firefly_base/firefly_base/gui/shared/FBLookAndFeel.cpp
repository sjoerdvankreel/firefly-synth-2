#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBThemedComponent.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

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

static bool
GetSliderModulationBounds(Slider const& s, double& minNorm, double& maxNorm)
{
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

static void
DrawCross(Graphics& g, Rectangle<float> const& bounds)
{
  Path p;
  p.startNewSubPath(bounds.getTopLeft());
  p.lineTo(bounds.getBottomRight());
  g.strokePath(p, PathStrokeType(3.0f, PathStrokeType::JointStyle::beveled, PathStrokeType::EndCapStyle::rounded));
  p = {};
  p.startNewSubPath(bounds.getBottomLeft());
  p.lineTo(bounds.getTopRight());
  g.strokePath(p, PathStrokeType(3.0f, PathStrokeType::JointStyle::beveled, PathStrokeType::EndCapStyle::rounded));
}

FBColorScheme const& 
FBLookAndFeel::FindColorSchemeFor(
  Component const& c) const
{
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
  }

  if (auto gui = c.findParentComponentOfClass<FBPlugGUI>())
  {
    int rtModuleIndex = -1;
    FBTabBarButton const* tabBarButton = nullptr;
    tabBarButton = dynamic_cast<FBTabBarButton const*>(&c);
    if (tabBarButton == nullptr)
      tabBarButton = c.findParentComponentOfClass<FBTabBarButton>();
    if (tabBarButton != nullptr && tabBarButton->isModuleTab)
      rtModuleIndex = gui->HostContext()->Topo()->moduleTopoToRuntime.at(tabBarButton->moduleIndices);
    else if (auto m = c.findParentComponentOfClass<FBModuleComponent>())
      rtModuleIndex = gui->HostContext()->Topo()->moduleTopoToRuntime.at({ m->ModuleIndex(), m->ModuleSlot() });
    if (rtModuleIndex != -1)
    {
      auto moduleIter = Theme().moduleColors.find(rtModuleIndex);
      if (moduleIter != Theme().moduleColors.end())
        return Theme().colorSchemes.at(moduleIter->second.colorScheme);
    }

    if (auto tc = c.findParentComponentOfClass<FBThemedComponent>())
    {
      auto componentIter = gui->HostContext()->Topo()->static_->themedComponents.find(tc->ComponentId());
      if (componentIter != gui->HostContext()->Topo()->static_->themedComponents.end())
      {
        auto schemeIter = Theme().componentColors.find(FBCleanTopoId(componentIter->second.id));
        if (schemeIter != Theme().componentColors.end())
          return Theme().colorSchemes.at(schemeIter->second.colorScheme);
      }      
    }
  }

  return Theme().defaultColorScheme;
}

void 
FBLookAndFeel::DrawLinearSliderExchangeThumb(
  Graphics& g, FBParamSlider& slider, FBColorScheme const& scheme,
  float thumbW, float thumbH, float thumbY, float exchangeValue) 
{
  auto layout = getSliderLayout(slider);
  int sliderRegionStart = layout.sliderBounds.getX();
  int sliderRegionSize = layout.sliderBounds.getWidth();
  float skewed = (float)ConvertValueFromSkewed(slider.Param()->static_, exchangeValue);
  float kx = static_cast<float>(sliderRegionStart + skewed * sliderRegionSize);
  g.setColour(scheme.paramHighlight);
  g.fillRoundedRectangle(kx - thumbW, thumbY, thumbW, thumbH, 2.0f);
  g.fillRoundedRectangle(kx, thumbY, thumbW, thumbH, 2.0f);
}

void 
FBLookAndFeel::DrawRotarySliderExchangeThumb(
  Graphics& g, FBParamSlider& slider, FBColorScheme const& scheme,
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
  g.setColour(scheme.paramHighlight);
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

void
FBLookAndFeel::DrawTabButtonPart(
  TabBarButton& button, Graphics& g,
  bool isMouseOver, bool /*isMouseDown*/,
  bool toggleState, bool centerText,
  bool isSeparator, std::string const& text,
  Rectangle<int> const& activeArea)
{
  bool isHeader = button.getTabbedButtonBar().getNumTabs() == 1;

  auto const& scheme = FindColorSchemeFor(button);
  if (isSeparator)
    g.setColour(scheme.paramSecondary);
  else if(toggleState)
    g.setColour(scheme.primary.darker(1.0f));
  else
    g.setColour(scheme.paramBackground);
  g.fillRoundedRectangle(activeArea.toFloat(), 3.0f);

  if(toggleState || isHeader || isMouseOver)
    g.setColour(scheme.primary);
  else
    g.setColour(scheme.sectionBorder);  
  g.drawRoundedRectangle(activeArea.toFloat(), 3.0f, 1.0f);

  TextLayout textLayout;
  const Rectangle<float> area(activeArea.toFloat());
  float length = area.getWidth();
  float depth = area.getHeight();
  auto textColor = isHeader? scheme.primary: scheme.text.darker(isSeparator || isMouseOver || toggleState ? 0.0f : scheme.dimDisabled);
  ::CreateTabTextLayout(button, length, textColor, FBGUIGetFont(), centerText, text, textLayout);
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
FBLookAndFeel::fillTextEditorBackground(
  Graphics& g, int width, int height, TextEditor& te)
{
  FBColorScheme const& scheme = FindColorSchemeFor(te);

  // todo the alerts
  if (dynamic_cast<AlertWindow*>(te.getParentComponent()) != nullptr)
  {
    g.setColour(te.findColour(TextEditor::backgroundColourId));
    g.fillRect(0, 0, width, height);
    g.setColour(te.findColour(TextEditor::outlineColourId));
    g.drawHorizontalLine(height - 1, 0.0f, static_cast<float> (width));
  }
  else
  {
    g.setColour(scheme.paramBackground);
    g.fillRoundedRectangle(te.getBounds().toFloat().withY(3.0f).withHeight(24.0f), 5.0f);
  }
}

void 
FBLookAndFeel::drawTextEditorOutline(
  Graphics&, int, int, TextEditor&)
{
}

void 
FBLookAndFeel::drawPopupMenuBackgroundWithOptions(
  juce::Graphics& g,
  int width, int height,
  const PopupMenu::Options& options)
{
  FBColorScheme const* scheme = &Theme().defaultColorScheme;
  if (options.getTargetComponent() != nullptr)
    scheme = &FindColorSchemeFor(*options.getTargetComponent());

  g.fillAll(scheme->paramBackground);
  g.setColour(scheme->sectionBorder);
  g.drawRect(0, 0, width, height);
}

void 
FBLookAndFeel::drawPopupMenuItemWithOptions(
  Graphics& g, const Rectangle<int>& area,
  bool isHighlighted,
  const PopupMenu::Item& item,
  const PopupMenu::Options& options)
{
  FBColorScheme const* scheme = &Theme().defaultColorScheme;
  if (options.getTopLevelTargetComponent() != nullptr)
    scheme = &FindColorSchemeFor(*options.getTopLevelTargetComponent());

  const auto colour = scheme->primary;
  const auto hasSubMenu = item.subMenu != nullptr
    && (item.itemID == 0 || item.subMenu->getNumItems() > 0);

  drawPopupMenuItem(g,
    area,
    item.isSeparator,
    item.isEnabled,
    isHighlighted,
    item.isTicked,
    hasSubMenu,
    item.text,
    item.shortcutKeyDescription,
    item.image.get(),
    &colour);
}

void 
FBLookAndFeel::drawPopupMenuItem(
  Graphics& g, const Rectangle<int>& area,
  const bool isSeparator, const bool isActive,
  const bool isHighlighted, const bool isTicked,
  const bool hasSubMenu, const juce::String& text,
  const String& /*shortcutKeyText*/,
  const Drawable* /*icon*/, const Colour* const textColourToUse)
{
  auto const& scheme = Theme().defaultColorScheme;
  Colour textColor = scheme.text;
  Colour selectedColor = textColourToUse != nullptr ? *textColourToUse : scheme.text;

  if (isSeparator)
  {
    auto r = area.reduced(5, 0);
    r.removeFromTop(roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));
    g.setColour(selectedColor);
    g.fillRect(r.removeFromTop(1));
    return;
  }

  auto r = area.reduced(1);
  if (isHighlighted && isActive)
  {
    g.setColour(scheme.sectionBackground);
    g.fillRect(r);
    g.setColour(selectedColor);
  }
  else
  {
    g.setColour(textColor.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
  }

  r.reduce(jmin(5, area.getWidth() / 20), 0);
  auto font = getPopupMenuFont();
  auto maxFontHeight = (float)r.getHeight() / 1.3f;
  if (font.getHeight() > maxFontHeight)
    font.setHeight(maxFontHeight);

  g.setFont(font.withHeight(FBGUIGetPopupMenuFontHeightFloat()));
  auto iconArea = r.removeFromLeft(roundToInt(maxFontHeight)).toFloat();
  if (isTicked)
  {
    float h = 6.0f;
    float y = (area.getHeight() - h) / 2.0f;
    DrawCross(g, Rectangle<float>(iconArea.getX() + 3.0f, y, h, h));
  }

  if (hasSubMenu)
  {
    auto arrowH = 0.6f * getPopupMenuFont().getAscent();

    auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
    auto halfH = static_cast<float> (r.getCentreY());

    Path path;
    path.startNewSubPath(x, halfH - arrowH * 0.5f);
    path.lineTo(x + arrowH * 0.6f, halfH);
    path.lineTo(x, halfH + arrowH * 0.5f);

    g.strokePath(path, PathStrokeType(2.0f));
  }

  r.removeFromRight(3);
  g.drawFittedText(text, r, Justification::centredLeft, 1);
}

void 
FBLookAndFeel::drawLabel(
  Graphics& g, Label& label)
{
  bool hasBackground = false;
  auto const& scheme = FindColorSchemeFor(label);
  if (dynamic_cast<FBAutoSizeLabel2*>(&label) ||
      dynamic_cast<FBLastTweakedLabel*>(&label))
  {
    hasBackground = true;
    g.setColour(scheme.paramBackground);
    g.fillRoundedRectangle(label.getLocalBounds().toFloat().reduced(5.0f), 5.0f);
  }

  auto colorText = scheme.text;
  if (auto b = label.findParentComponentOfClass<ComboBox>())
    colorText = scheme.primary.darker(b->isEnabled() ? 0.0f : scheme.dimDisabled);

  g.setFont(getLabelFont(label));
  g.setColour(colorText);
  auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
  if (hasBackground)
    textArea = textArea.reduced(5);
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
  auto cornerSize = 3.0f;
  auto const& scheme = FindColorSchemeFor(box);
  Rectangle<int> boxBounds(2, 2, width - 4, height - 4);

  g.setColour(scheme.paramBackground);
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);
  g.setColour(scheme.paramSecondary);
  g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
  auto* paramCombo = dynamic_cast<FBParamComboBox*>(&box);
  if (paramCombo != nullptr && paramCombo->IsHighlightTweaked())
  {
    g.setColour(scheme.paramHighlight);
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
  }
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

  g.setColour(scheme.paramBackground);
  g.fillRoundedRectangle(tickBounds, 2.0f);
  g.setColour(scheme.paramSecondary);
  g.drawRoundedRectangle(tickBounds, 2.0f, 1.0f);
  auto* paramToggle = dynamic_cast<FBParamToggleButton*>(&component);
  if (paramToggle != nullptr && paramToggle->IsHighlightTweaked())
  {
    g.setColour(scheme.paramHighlight);
    g.drawRoundedRectangle(tickBounds, 2.0f, 1.0f);
  }

  if (ticked)
  {
    g.setColour(scheme.primary.darker(component.isEnabled()? 0.0f: scheme.dimDisabled));
    DrawCross(g, tickBounds.reduced(6.0f, 6.0f));
  }
}

void
FBLookAndFeel::drawLinearSlider(
  Graphics& g,
  int x, int y, int width, int height,
  float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
  Slider::SliderStyle /*style*/, Slider& slider)
{
  auto const& scheme = FindColorSchemeFor(slider);
  auto trackWidth = 6.0f;

  FB_ASSERT(slider.isHorizontal());
  Path backgroundTrackFull;
  Point<float> startPointFull((float)x, (float)y + (float)height * 0.5f);
  Point<float> endPointFull((float)(width + x), startPointFull.y);
  backgroundTrackFull.startNewSubPath(startPointFull);
  backgroundTrackFull.lineTo(endPointFull);
  g.setColour(scheme.paramBackground);
  g.strokePath(backgroundTrackFull, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

  Path valueTrack;
  Point<float> minPoint, maxPoint;
  auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
  auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
  minPoint = startPointFull;
  maxPoint = { kx, ky };

  auto paramSlider = dynamic_cast<FBParamSlider*>(&slider);
  if (paramSlider != nullptr)
    if (paramSlider->Param()->static_.NonRealTime().DisplayAsBipolar())
    {
      auto centerPoint = Point<float>((float)x + width * 0.5f, (float)y + (float)height * 0.5f);
      float sliderPosNorm = (sliderPos - (float)x) / width;
      if (sliderPosNorm > 0.5f)
        minPoint = centerPoint;
      else
      {
        minPoint = maxPoint;
        maxPoint = centerPoint;
      }
    }

  valueTrack.startNewSubPath(minPoint);
  valueTrack.lineTo(maxPoint);
  g.setColour(scheme.paramSecondary);
  g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

  double modMin, modMax;
  if (GetSliderModulationBounds(slider, modMin, modMax))
  {
    Path backgroundTrackMod;
    Point<float> startPointMod((float)(x + width * modMin), (float)y + (float)height * 0.5f);
    Point<float> endPointMod((float)(width * modMax + x), startPointMod.y);
    backgroundTrackMod.startNewSubPath(startPointMod);
    backgroundTrackMod.lineTo(endPointMod);
    g.setColour(scheme.paramHighlight);
    g.strokePath(backgroundTrackMod, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
  }

  float thumbW = 4.0f;
  float thumbH = (float)height * 0.6f;
  float thumbY = (float)y + (float)height * 0.2f;
  if (paramSlider != nullptr)
  {
    auto paramActive = paramSlider->ParamActiveExchangeState();
    if (paramActive.active)
    {
      DrawLinearSliderExchangeThumb(g, *paramSlider, scheme, thumbW, thumbH, thumbY, paramActive.minValue);
      if (paramSlider->Param()->static_.IsVoice())
        DrawLinearSliderExchangeThumb(g, *paramSlider, scheme, thumbW, thumbH, thumbY, paramActive.maxValue);
    }
  }

  g.setColour(scheme.primary.darker(slider.isEnabled()? 0.0f: scheme.dimDisabled));
  g.fillRoundedRectangle(kx - thumbW, thumbY, thumbW, thumbH, 2.0f);
  g.fillRoundedRectangle(kx, thumbY, thumbW, thumbH, 2.0f);
  if (paramSlider != nullptr && paramSlider->IsHighlightTweaked())
  {
    g.setColour(scheme.paramHighlight);
    g.fillRoundedRectangle(kx - thumbW, thumbY, thumbW, thumbH, 2.0f);
    g.fillRoundedRectangle(kx, thumbY, thumbW, thumbH, 2.0f);
  }
}

void 
FBLookAndFeel::drawButtonBackground(
  juce::Graphics& g, juce::Button& button, 
  const juce::Colour& /*backgroundColour*/,
  bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
  auto cornerSize = 5.0f;
  auto bounds = button.getLocalBounds().toFloat().reduced(3.0f, 3.0f);
  auto const& scheme = FindColorSchemeFor(button);

  auto color = scheme.primary;
  g.setColour(scheme.primary.darker(1.0f).brighter(shouldDrawButtonAsDown? 0.4f: 0.0f));
  g.fillRoundedRectangle(bounds, cornerSize);
  g.setColour(scheme.primary);
  g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
  if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
  {
    g.setColour(scheme.paramHighlight);
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
  x -= 6;
  y -= 6;
  width += 12;
  height += 12;

  auto const& scheme = FindColorSchemeFor(slider);
  auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);
  auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
  auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  auto lineW = jmin(8.0f, radius * 0.5f);
  auto arcRadius = radius - lineW * 0.5f;

  Path backgroundArc;
  backgroundArc.addCentredArc(
    bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
    0.0f, rotaryStartAngle, rotaryEndAngle, true);
  g.setColour(scheme.paramBackground);
  g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::butt));

  Path valueArc;
  float trackEndAngle = toAngle;
  float trackStartAngle = rotaryStartAngle;
  if (auto ps = dynamic_cast<FBParamSlider*>(&slider))
  {
    bool bipolar = ps->Param()->static_.NonRealTime().DisplayAsBipolar();
    if (bipolar)
    {
      float centerAngle = rotaryStartAngle + 0.5f * (rotaryEndAngle - rotaryStartAngle);
      if (sliderPos > 0.5f)
        trackStartAngle = centerAngle;
      else
      {
        trackStartAngle = toAngle;
        trackEndAngle = centerAngle;
      }
    }
  }
  valueArc.addCentredArc(
    bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius,
    0.0f, trackStartAngle, trackEndAngle, true);
  g.setColour(scheme.paramSecondary);
  g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::butt));

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
    g.setColour(scheme.paramHighlight);
    g.strokePath(modArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::butt));
  }

  FBParamSlider* paramSlider = dynamic_cast<FBParamSlider*>(&slider);
  if (paramSlider != nullptr)
  {
    auto paramActive = paramSlider->ParamActiveExchangeState();
    if (paramActive.active)
    {
      DrawRotarySliderExchangeThumb(g, *paramSlider, scheme, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.minValue);
      if (paramSlider->Param()->static_.IsVoice())
        DrawRotarySliderExchangeThumb(g, *paramSlider, scheme, x, y, width, height, rotaryStartAngle, rotaryEndAngle, paramActive.maxValue);
    }
  }

  auto thumbWidth = lineW * 2.0f;
  Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
    bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
  g.setColour(scheme.primary.darker(slider.isEnabled()? 0.0f: scheme.dimDisabled));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));

  if (paramSlider != nullptr && paramSlider->IsHighlightTweaked())
  { 
    g.setColour(scheme.paramHighlight);
    g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
  } 
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
  // Tooltip window doesnt provide the target component, so wing it.
  // Won't be 100% accurate, but worst that can happen is we pick up the wrong color scheme.
  bool newCompIsParam = false;
  auto const* scheme = &Theme().defaultColorScheme;
  const auto mouseSource = Desktop::getInstance().getMainMouseSource();
  auto* newComp = mouseSource.isTouch() ? nullptr : mouseSource.getComponentUnderMouse();
  if (newComp != nullptr)
  {
    scheme = &FindColorSchemeFor(*newComp);
    newCompIsParam |= dynamic_cast<FBParamControl*>(newComp) != nullptr;
    newCompIsParam |= dynamic_cast<FBGUIParamControl*>(newComp) != nullptr;
    newCompIsParam |= dynamic_cast<FBParamControl*>(newComp->findParentComponentOfClass<ComboBox>()) != nullptr;
    newCompIsParam |= dynamic_cast<FBGUIParamControl*>(newComp->findParentComponentOfClass<ComboBox>()) != nullptr;
  }

  auto cornerSize = 3.0f;
  Rectangle<int> bounds(width, height);
  g.setColour(scheme->background);
  g.fillRect(bounds.toFloat());
  g.setColour(scheme->paramBackground);
  g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
  g.setColour(scheme->primary);
  g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

  int i = 0;
  float pad = 3.0f;
  float fontSize = 13.0f;
  auto trimmed = text.trim();
  float textHeight = FBGUIGetFontHeightFloat() + 2.0f;
  auto lines = FBStringSplit(trimmed.toStdString(), "\r\n");
  auto textBounds = Rectangle<float>(pad, pad, width - 2.0f * pad, textHeight);
  g.setFont(Font(FontOptions(fontSize, Font::bold).withMetricsKind(getDefaultMetricsKind())));
  while (lines.size() > 0)
  {
    if (lines[0].size() > 0)
    {
      if(i == 0 && newCompIsParam)
        g.setColour(scheme->primary);
      else
        g.setColour(scheme->text);
      g.drawText(lines[0], textBounds, Justification::left, false);
    }
    lines.erase(lines.begin());
    textBounds.translate(0.0f, textHeight);
    i++;
  }
}

void 
FBLookAndFeel::drawTabbedButtonBarBackground(
  TabbedButtonBar& b, Graphics& g)
{
  auto const& scheme = FindColorSchemeFor(b);
  g.setColour(scheme.paramBackground);
  g.fillRoundedRectangle(b.getBounds().toFloat(), 3.0f);
}

void
FBLookAndFeel::drawTabButton(
  TabBarButton& button, Graphics& g,
  bool isMouseOver, bool isMouseDown)
{
  bool big = {};
  bool large = {};
  bool centerText = {};
  std::string separatorText = {};
  FBTabBarButton* fbButton = dynamic_cast<FBTabBarButton*>(&button);
  if (fbButton != nullptr)
  {
    large = fbButton->large;
    centerText = fbButton->centerText;
    if(auto fbModuleButton = dynamic_cast<FBModuleTabBarButton*>(&button))
      separatorText = fbModuleButton->GetSeparatorText();
  }

  if (auto fbt = button.findParentComponentOfClass<FBAutoSizeTabComponent>())
  {
    big = fbt->Big();
    centerText |= big;
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
    DrawTabButtonPart(button, g, isMouseOver, isMouseDown, toggleState, centerText, false, buttonText, activeArea.reduced(1, 0));
    return;
  }

  int size = large ? TabSizeLarge : TabSizeSmall;
  auto separatorArea = Rectangle<int>(activeArea.getX() + 1, activeArea.getY(), TabSizeLarge - 2, activeArea.getHeight());
  DrawTabButtonPart(button, g, false, false, false, false, true, separatorText, separatorArea);
  auto buttonArea = Rectangle<int>(activeArea.getX() + TabSizeLarge + 1, activeArea.getY(), size - 2, activeArea.getHeight());
  DrawTabButtonPart(button, g, isMouseOver, isMouseDown, toggleState, centerText, false, buttonText, buttonArea);
}