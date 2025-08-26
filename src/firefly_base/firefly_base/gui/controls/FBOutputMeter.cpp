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
  int const barCount = 20;
  int const gapCount = barCount - 1;
  
  float const gapSize = 2.0f;
  float const barHeight = 10.0f;
  float const totalGapSize = gapCount * gapSize;
  float const barY = (getLocalBounds().getHeight() - barHeight) * 0.5f;
  float const barSize = (getLocalBounds().getWidth() - totalGapSize) / barCount;

  double barsFilledFloat = _valueNormalized * barCount;
  int barsFilled = (int)std::round(barsFilledFloat);

  int i = 0;
  g.setColour(Colours::lightgrey);
  for (; i < barsFilled; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
  g.setColour(Colours::darkgrey);
  for (; i < barCount; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
}