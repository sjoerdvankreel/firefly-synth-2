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
  g.fillAll(Colours::aliceblue);
  g.setColour(Colours::red);
  g.drawText(std::to_string(_valueNormalized), getLocalBounds(), Justification::centred, false);
}