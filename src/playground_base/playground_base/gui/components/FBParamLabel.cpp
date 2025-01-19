#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/components/FBParamLabel.hpp>

#include <string>

using namespace juce;

FBParamLabel::
FBParamLabel(FBRuntimeParam const* param):
Label()
{
  setText(param->shortName, dontSendNotification);
}