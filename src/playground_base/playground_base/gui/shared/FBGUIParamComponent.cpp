#include <playground_base/gui/shared/FBGUIParamComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

FBGUIParamComponent::
FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies),
_param(param) {}