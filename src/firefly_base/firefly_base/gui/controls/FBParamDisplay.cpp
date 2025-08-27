#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

static int const MeterBarCount = 20;

using namespace juce;

FBParamDisplayLabel::
FBParamDisplayLabel(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param, 
  std::string const& defaultText,
  std::string const& maxWidthText):
Label(),
FBParamControl(plugGUI, param),
_maxTextWidth(FBGUIGetStringWidthCached(maxWidthText))
{
  setText(defaultText, dontSendNotification);
}

void
FBParamDisplayLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

int
FBParamDisplayLabel::FixedWidth(int /*height*/) const
{
  return _maxTextWidth + getBorderSize().getLeftAndRight();
}

String
FBParamDisplayLabel::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBParamDisplayLabel::SetValueNormalizedFromHost(double normalized)
{
  setText(_param->NormalizedToTextWithUnit(false, normalized), dontSendNotification);
}

FBParamDisplayMeter::
FBParamDisplayMeter(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param):
Component(),
FBParamControl(plugGUI, param) {}

void
FBParamDisplayMeter::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBParamDisplayMeter::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBParamDisplayMeter::SetValueNormalizedFromHost(double normalized)
{
  double barsFilledFloat = normalized * MeterBarCount;
  int fillCount = (int)std::round(barsFilledFloat);
  if (_fillCount != fillCount)
  {
    _fillCount = fillCount;
    repaint();
  }
}

void 
FBParamDisplayMeter::paint(Graphics& g)
{
  float const gapSize = 2.0f;
  float const barHeight = 10.0f;
  int const gapCount = MeterBarCount - 1;
  float const totalGapSize = gapCount * gapSize;
  float const barY = (getLocalBounds().getHeight() - barHeight) * 0.5f;
  float const barSize = (getLocalBounds().getWidth() - totalGapSize) / MeterBarCount;

  int i = 0;
  g.setColour(Colours::lightgrey);
  for (; i < _fillCount; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
  g.setColour(Colours::darkgrey);
  for (; i < MeterBarCount; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
}