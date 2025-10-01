#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBAutoSizeButton::
FBAutoSizeButton(std::string const& text):
TextButton(),
IFBHorizontalAutoSize(),
_textWidth(FBGUIGetStringWidthCached(text))
{
  setButtonText(text);
}

int 
FBAutoSizeButton::FixedHeight() const
{
  return 24;
}

int 
FBAutoSizeButton::FixedWidth(int /*height*/) const
{
  return _textWidth + 16;
}

FBParamLinkedButton::
FBParamLinkedButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string const& text):
FBAutoSizeButton(text),
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies) {}

void
FBParamLinkedButton::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

FBParamValueLinkedButton::
~FBParamValueLinkedButton()
{
  _plugGUI->RemoveParamListener(this);
}

FBParamValueLinkedButton::
FBParamValueLinkedButton(
  FBPlugGUI* plugGUI, FBRuntimeParam const* param,
  std::string const& text, std::function<bool(int)> enabledIf):
FBAutoSizeButton(text),
_plugGUI(plugGUI),
_param(param),
_enabledIf(enabledIf)
{
  plugGUI->AddParamListener(this);
  setEnabled(_enabledIf(_plugGUI->HostContext()->GetAudioParamDiscrete(param->topoIndices)));
}

void
FBParamValueLinkedButton::AudioParamChanged(int index, double /*normalized*/, bool /*changedFromUI*/)
{
  if(_param->runtimeParamIndex == index)
    setEnabled(_enabledIf(_plugGUI->HostContext()->GetAudioParamDiscrete(_param->topoIndices)));
}