#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBParamLabel::
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBAutoSizeLabel(param->displayName),
FBParamComponent(plugGUI, param) {}

void 
FBParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}