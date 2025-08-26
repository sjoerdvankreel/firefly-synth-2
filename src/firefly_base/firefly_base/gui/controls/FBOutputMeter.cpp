#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBOutputMeter.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBOutputParamMeter::
FBOutputParamMeter(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param):
Component(),
FBParamControl(plugGUI, param) {}

void
FBOutputParamMeter::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBOutputParamMeter::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBOutputParamMeter::SetValueNormalizedFromHost(double normalized)
{
  _valueNormalized = normalized;
  repaint();
}

void 
FBOutputParamMeter::paint(Graphics& g)
{
  int const gapSize = 3;
  int const barCount = 20;
  int const gapCount = barCount - 1;
  int const totalGapSize = gapCount * gapSize;
  int const barSize = (getLocalBounds().getWidth() - totalGapSize) / barCount;

  double barsFilledFloat = _valueNormalized * barCount;
  int barsFilled = (int)std::round(barsFilledFloat);

  int i = 0;
  g.setColour(Colours::lightgrey);
  for (; i < barsFilled; i++)
    g.fillRect(i * (barSize + gapSize), 0, barSize, getHeight());
  g.setColour(Colours::darkgrey);
  for (; i < barCount; i++)
    g.fillRect(i * (barSize + gapSize), 0, barSize, getHeight());
}