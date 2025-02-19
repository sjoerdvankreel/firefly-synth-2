#include <playground_base/gui/shared/FBGUIParamComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeGUIParam.hpp>

FBGUIParamComponent::
FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.module, param->static_.dependencies),
_param(param) {}