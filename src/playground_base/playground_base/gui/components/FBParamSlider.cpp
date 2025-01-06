#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>

using namespace juce;

FBParamSlider::
FBParamSlider(
  FBRuntimeParam const* param,
  IFBHostGUIContext* context,
  Slider::SliderStyle style):
Slider(style, Slider::TextBoxBelow),
_context(context),
_param(param) 
{
  setRange(0.0, 1.0);
  float normalized = _context->GetParamNormalized(param->runtimeParamIndex);
  setValue(normalized, juce::dontSendNotification);
}

void 
FBParamSlider::stoppedDragging()
{
  _context->EndParamChange(_param->runtimeParamIndex);
}

void 
FBParamSlider::startedDragging()
{
  _context->BeginParamChange(_param->runtimeParamIndex);
}

void
FBParamSlider::valueChanged()
{
  _context->PerformParamEdit(_param->runtimeParamIndex, getValue());
}