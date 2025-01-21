#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamsDependent(plugGUI, param->static_.relevant),
_param(param) {}