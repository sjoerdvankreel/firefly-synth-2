#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed):
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies),
_isThemed(isThemed),
_param(param) {}

FBColorScheme const* 
FBParamComponent::GetScheme(FBTheme const& theme) const
{  
  if (!_isThemed)
    return nullptr;
  int rtModuleIndex = Param()->runtimeModuleIndex;
  auto moduleIter = theme.moduleColors.find(rtModuleIndex);
  if (moduleIter != theme.moduleColors.end())
  {
    int rtParamIndex = Param()->runtimeParamIndex;
    auto paramIter = moduleIter->second.audioParamColorSchemes.find(rtParamIndex);
    if (paramIter == moduleIter->second.audioParamColorSchemes.end())
      return &theme.colorSchemes.at(moduleIter->second.colorScheme);
    return &theme.colorSchemes.at(paramIter->second);
  }
  return nullptr;
}

FBGUIParamComponent::
FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed):
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies),
_isThemed(isThemed),
_param(param) {}

FBColorScheme const*
FBGUIParamComponent::GetScheme(FBTheme const& theme) const
{
  if (!_isThemed)
    return nullptr;
  int rtModuleIndex = Param()->runtimeModuleIndex;
  auto moduleIter = theme.moduleColors.find(rtModuleIndex);
  if (moduleIter != theme.moduleColors.end())
  {
    int rtParamIndex = Param()->runtimeParamIndex;
    auto paramIter = moduleIter->second.guiParamColorSchemes.find(rtParamIndex);
    if (paramIter == moduleIter->second.guiParamColorSchemes.end())
      return &theme.colorSchemes.at(moduleIter->second.colorScheme);
    return &theme.colorSchemes.at(paramIter->second);
  }
  return nullptr;
}

FBGUIParamControl::
FBGUIParamControl(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed):
FBGUIParamComponent(plugGUI, param, isThemed) {}

FBParamControl::
FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed):
FBParamComponent(plugGUI, param, isThemed) {}

void
FBParamControl::StartFlash()
{
  if (_isFlash)
    return;
  _isFlash = true;
  dynamic_cast<juce::Component*>(this)->repaint();
  juce::Timer::callAfterDelay(1000, [this]() {
    _isFlash = false;
    dynamic_cast<juce::Component*>(this)->repaint(); });
}

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