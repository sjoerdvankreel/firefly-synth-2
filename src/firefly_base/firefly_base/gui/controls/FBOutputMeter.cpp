#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBOutputMeter.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

static int const BarCount = 20;

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
  double barsFilledFloat = normalized * BarCount;
  int fillCount = (int)std::round(barsFilledFloat);
  if (_fillCount != fillCount)
  {
    _fillCount = fillCount;
    //repaint(); // TODO why so slow??
  }
}

void 
FBOutputParamMeter::paint(Graphics&)
{
#if 0 // TODO SLOOOOOOOOOOOOOOOOOW
  float const gapSize = 2.0f;
  float const barHeight = 10.0f;
  int const gapCount = BarCount - 1;
  float const totalGapSize = gapCount * gapSize;
  float const barY = (getLocalBounds().getHeight() - barHeight) * 0.5f;
  float const barSize = (getLocalBounds().getWidth() - totalGapSize) / BarCount;

  int i = 0;
  g.setColour(Colours::lightgrey);
  for (; i < _fillCount; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
  g.setColour(Colours::darkgrey);
  for (; i < BarCount; i++)
    g.fillRect(i * (barSize + gapSize), barY, barSize, barHeight);
#endif
}