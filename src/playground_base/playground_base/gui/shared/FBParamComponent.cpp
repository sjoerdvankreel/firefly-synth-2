#include <playground_base/gui/shared/FBParamComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies),
_param(param) {}

FBGUIParamComponent::
FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies),
_param(param) {}

FBParamControl::
FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamComponent(plugGUI, param) {}

FBGUIParamControl::
FBGUIParamControl(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBGUIParamComponent(plugGUI, param) {}