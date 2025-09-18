#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
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