#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBAutoSizeButton::
FBAutoSizeButton(FBPlugGUI* plugGUI, std::string const& text):
TextButton(),
IFBHorizontalAutoSize(),
_plugGUI(plugGUI),
_text(text)
{
  setButtonText(text);
}

int 
FBAutoSizeButton::FixedHeight() const
{
  return FBPrimaryHeight;
}

int 
FBAutoSizeButton::FixedWidth(int /*height*/) const
{
  return FBGetLookAndFeelFor(_plugGUI)->GetStringWidthCached(_text) + 14;
}

FBParamLinkedButton::
FBParamLinkedButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string const& text):
FBAutoSizeButton(plugGUI, text),
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
FBAutoSizeButton(plugGUI, text),
_plugGUI(plugGUI),
_param(param),
_enabledIf(enabledIf)
{
  plugGUI->AddParamListener(this);
  ToggleEnabled();
}

void 
FBParamValueLinkedButton::ToggleEnabled()
{
  if (_param->static_.type == FBParamType::Discrete)
    setEnabled(_enabledIf(_plugGUI->HostContext()->GetAudioParamDiscrete(_param->topoIndices)));
  else if (_param->static_.type == FBParamType::List)
    setEnabled(_enabledIf(_plugGUI->HostContext()->GetAudioParamList<int>(_param->topoIndices)));
  else
    FB_ASSERT(false);
}

void
FBParamValueLinkedButton::AudioParamChanged(int index, double /*normalized*/, bool /*changedFromUI*/)
{
  if (_param->runtimeParamIndex == index)
    ToggleEnabled();
}