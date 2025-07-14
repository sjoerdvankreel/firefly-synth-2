#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBAutoSizeComboBox::
FBAutoSizeComboBox(PopupMenu const& rootMenu):
ComboBox()
{
  *getRootMenu() = rootMenu;
  for (int i = 0; i < getNumItems(); i++)
    _maxTextWidth = std::max(_maxTextWidth, 
      FBGUIGetStringWidthCached(getItemText(i).toStdString()));
}

int 
FBAutoSizeComboBox::FixedHeight() const
{
  return 24;
}

int
FBAutoSizeComboBox::FixedWidth(int /*height*/) const
{
  return _maxTextWidth + 14;
}

FBGUIParamComboBox::
FBGUIParamComboBox(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBAutoSizeComboBox(param->static_.ItemsNonRealTime().MakePopupMenu(param->topoIndices.module.index)),
FBGUIParamControl(plugGUI, param)
{
  SetValueNormalizedFromPlug(plugGUI->HostContext()->GetGUIParamNormalized(param->runtimeParamIndex));
}

void
FBGUIParamComboBox::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBGUIParamComboBox::getTooltip()
{
  return _plugGUI->GetTooltipForGUIParam(_param->runtimeParamIndex);
}

void
FBGUIParamComboBox::SetValueNormalizedFromPlug(double normalized)
{
  double plain = _param->static_.NonRealTime().NormalizedToPlain(normalized);
  setSelectedId(static_cast<int>(plain) + 1, dontSendNotification);
}

void
FBGUIParamComboBox::valueChanged(Value& /*value*/)
{
  double normalized = _param->static_.NonRealTime().PlainToNormalized(getSelectedId() - 1);
  _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, normalized);
  _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, normalized);
  setTooltip(getTooltip()); // hack but needed
}

FBParamComboBox::
FBParamComboBox(FBPlugGUI* plugGUI, FBRuntimeParam const* param) :
  FBAutoSizeComboBox(param->static_.ItemsNonRealTime().MakePopupMenu(param->topoIndices.module.index)),
  FBParamControl(plugGUI, param)
{
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex));
}

void
FBParamComboBox::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBParamComboBox::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void 
FBParamComboBox::showPopup()
{
  // need to catch real user input for the undo state, not all kinds of async callbacks
  // this will cause some spurious undo items if user opens popup but not changes it
  _plugGUI->HostContext()->UndoState().Snapshot("Change " + _param->longName);
  ComboBox::showPopup();
}

void
FBParamComboBox::SetValueNormalizedFromHost(double normalized)
{
  double plain = _param->static_.NonRealTime().NormalizedToPlain(normalized);
  setSelectedId(static_cast<int>(plain) + 1, dontSendNotification);
}

void
FBParamComboBox::valueChanged(Value& /*value*/)
{
  double normalized = _param->static_.NonRealTime().PlainToNormalized(getSelectedId() - 1);
  _plugGUI->HostContext()->PerformImmediateAudioParamEdit(_param->runtimeParamIndex, normalized);
  _plugGUI->AudioParamNormalizedChangedFromUI(_param->runtimeParamIndex, normalized);
  setTooltip(getTooltip()); // hack but needed
}