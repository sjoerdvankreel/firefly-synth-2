#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>

using namespace juce;

FBParamSlider::
FBParamSlider(
  IFBHostGUIContext* context, 
  FBRuntimeParam const* param,
  Slider::SliderStyle style):
Slider(style, Slider::NoTextBox),
_context(context),
_param(param) {}

void 
FBParamSlider::stoppedDragging()
{
  _context->EndParamChange(_param->runtimeModuleIndex);
}

void 
FBParamSlider::startedDragging()
{
  _context->BeginParamChange(_param->runtimeModuleIndex);
}

void
FBParamSlider::valueChanged()
{
  _context->SetParamNormalized(_param->runtimeParamIndex, getValue());
}