#include <playground_base/gui/shared/FBParamComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.module, param->static_.dependencies),
_param(param) {}