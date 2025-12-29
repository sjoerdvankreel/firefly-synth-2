#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBAutoSizeLabel::
FBAutoSizeLabel(std::string const& text, bool centred):
Label(),
IFBHorizontalAutoSize(),
_textWidth(FBGUIGetStringWidthCached(text))
{
  setText(text, dontSendNotification);
  setBorderSize({ 1, 2, 1, 2 });
  if (centred)
    setJustificationType(Justification::centred);
}

int 
FBAutoSizeLabel::FixedHeight() const
{
  return 20;
}

int 
FBAutoSizeLabel::FixedWidth(int /*height*/) const
{
  return getBorderSize().getLeftAndRight() + _textWidth;
}

FBAutoSizeLabel2::
FBAutoSizeLabel2(int fixedWidth) :
Label(),
IFBHorizontalAutoSize(),
_fixedWidth(fixedWidth) { }

int
FBAutoSizeLabel2::FixedHeight() const
{
  return 20;
}

int
FBAutoSizeLabel2::FixedWidth(int /*height*/) const
{
  return getBorderSize().getLeftAndRight() + _fixedWidth;
}

FBGUIParamLabel::
FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed) :
FBAutoSizeLabel(param->displayName),
FBGUIParamComponent(plugGUI, param, isThemed) {}

void
FBGUIParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

FBParamLabel::
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed):
FBAutoSizeLabel(param->displayName),
FBParamComponent(plugGUI, param, isThemed) {}

void 
FBParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

FBParamLinkedLabel::
FBParamLinkedLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string text):
FBAutoSizeLabel(text),
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies) {}

void 
FBParamLinkedLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}