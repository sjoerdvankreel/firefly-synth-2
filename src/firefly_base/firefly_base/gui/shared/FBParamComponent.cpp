#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies),
_param(param) {}

FBGUIParamComponent::
FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies),
_param(param) {}

FBGUIParamControl::
FBGUIParamControl(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBGUIParamComponent(plugGUI, param) {}

FBParamControl::
FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamComponent(plugGUI, param) {}

bool 
FBParamControl::IsHighlightTweaked() const
{
  double const epsilon = 1.0e-4;
  if (!_plugGUI->HighlightTweaked())
    return false;
  double default_ = _param->DefaultNormalizedByText();
  double norm = _plugGUI->HostContext()->GetAudioParamNormalized(_param->runtimeParamIndex);
  return std::abs(default_ - norm) >= epsilon;
}