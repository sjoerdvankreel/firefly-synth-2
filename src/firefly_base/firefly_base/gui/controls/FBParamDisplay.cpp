#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

static int const MeterBarCount = 25;

using namespace juce;

FBParamDisplayLabel::
FBParamDisplayLabel(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param, 
  std::string const& maxWidthText,
  bool isThemed):
Label(),
FBParamControl(plugGUI, param, isThemed),
_maxTextWidth(FBGUIGetStringWidthCached(maxWidthText))
{
  double normalized = plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex);
  auto text = plugGUI->HostContext()->Topo()->audio.params[param->runtimeParamIndex].NormalizedToTextWithUnit(false, normalized);
  setText(text, dontSendNotification);
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

void
FBParamDisplayLabel::SetValueNormalizedFromHost(double normalized)
{
  setText(_param->NormalizedToTextWithUnit(false, normalized), dontSendNotification);
}

FBParamDisplayMeter::
FBParamDisplayMeter(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param,
  bool isThemed):
Component(),
FBParamControl(plugGUI, param, isThemed) {}

void
FBParamDisplayMeter::parentHierarchyChanged()
{
  ParentHierarchyChanged();
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
  auto const& scheme = FBGetLookAndFeel()->FindColorSchemeFor(*this);

  float const gapSize = 2.0f;
  float const barHeight = 10.0f;
  int const gapCount = MeterBarCount - 1;
  float const totalGapSize = gapCount * gapSize;
  float const barY = (getLocalBounds().getHeight() - barHeight) * 0.5f;
  float const barSize = (getLocalBounds().getWidth() - totalGapSize) / MeterBarCount;

  int i = 0;
  g.setColour(scheme.meterFill);
  for (; i < _fillCount; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
  g.setColour(scheme.meterTrack);
  for (; i < MeterBarCount; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
  for (i = MeterBarCount - 3; i < _fillCount; i++)
  {  
    g.setColour(scheme.meterAlert.withAlpha(0.33f * (1 + i - (MeterBarCount - 3))));
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
  }
}