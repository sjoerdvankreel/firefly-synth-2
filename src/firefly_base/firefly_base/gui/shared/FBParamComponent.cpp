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
  auto mode = _plugGUI->HighlightTweakedMode();
  if (mode == FBHighlightTweakMode::Off)
    return false;
  double compare = 0.0;
  double norm = _plugGUI->HostContext()->GetAudioParamNormalized(_param->runtimeParamIndex);
  switch (mode)
  {
  case FBHighlightTweakMode::Default: compare = _param->DefaultNormalizedByText(); break;
  case FBHighlightTweakMode::Patch: compare = *_plugGUI->HostContext()->PatchState().Params()[_param->runtimeParamIndex]; break;
  case FBHighlightTweakMode::Session: compare = *_plugGUI->HostContext()->SessionState().Params()[_param->runtimeParamIndex]; break;
  default: FB_ASSERT(false); break;
  }
  return std::abs(compare - norm) >= epsilon;
}