#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBAutoSizeLabel::
FBAutoSizeLabel(FBPlugGUI* plugGUI, std::string const& text, bool centred, bool isPrimary, bool small):
Label(),
IFBHorizontalAutoSize(),
_plugGUI(plugGUI),
_text(text),
_isPrimary(isPrimary),
_small(small)
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
  return getBorderSize().getLeftAndRight() + FBGetLookAndFeelFor(_plugGUI)->GetStringWidthCached(_text) + (_isPrimary ? 2 : 0);
}

FBAutoSizeLabel2::
FBAutoSizeLabel2(bool hasBackground, int fixedWidth) :
Label(),
IFBHorizontalAutoSize(),
_hasBackground(hasBackground),
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
FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param, bool isThemed, bool isPrimary) :
FBAutoSizeLabel(param->displayName, false, isPrimary),
FBGUIParamComponent(plugGUI, param, isThemed) {}

void
FBGUIParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

FBParamLabel::
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isThemed, bool isPrimary):
FBAutoSizeLabel(param->displayName, false, isPrimary),
FBParamComponent(plugGUI, param, isThemed) {}

void 
FBParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

FBParamLinkedLabel::
FBParamLinkedLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, bool isPrimary, std::string text):
FBAutoSizeLabel(text, false, isPrimary),
FBParamsDependent(plugGUI, param->topoIndices.param.slot, param->topoIndices.module, param->static_.dependencies) {}

void 
FBParamLinkedLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}