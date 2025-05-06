#include <playground_base/gui/controls/FBGUIParamLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeGUIParam.hpp>

using namespace juce;

FBGUIParamLabel::
FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBAutoSizeLabel(param->displayName),
FBGUIParamComponent(plugGUI, param) {}

void
FBGUIParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}