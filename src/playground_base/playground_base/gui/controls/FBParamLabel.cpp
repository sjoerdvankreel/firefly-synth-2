#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeAudioParam.hpp>

using namespace juce;

FBParamLabel::
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeAudioParam const* param):
FBAutoSizeLabel(param->shortName),
FBParamComponent(plugGUI, param) {}

void 
FBParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}