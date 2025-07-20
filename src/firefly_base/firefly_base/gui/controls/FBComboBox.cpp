#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
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

void 
FBParamComboBox::EnableItems(PopupMenu* menu, int runtimeSourceValue)
{
  PopupMenu::MenuItemIterator iter(*menu);
  while (iter.next())
  {
    if (iter.getItem().subMenu != nullptr)
      EnableItems(iter.getItem().subMenu.get(), runtimeSourceValue);
    else
      iter.getItem().isEnabled = _param->static_.List().targetEnabledSelector(runtimeSourceValue, iter.getItem().itemID - 1);
  }
}

void
FBParamComboBox::showPopup()
{
  // need to catch real user input for the undo state, not all kinds of async callbacks
  // this will cause some spurious undo items if user opens popup but not changes it
  _plugGUI->HostContext()->UndoState().Snapshot("Change " + _param->longName);

  // dependent comboboxes
  if (_param->static_.type == FBParamType::List && _param->static_.List().targetEnabledSelector != nullptr)
  {
    auto const& list = _param->static_.List();
    auto const* topo = _plugGUI->HostContext()->Topo();
    FB_ASSERT(list.targetEnabledSource != -1);
    FBParamTopoIndices sourceIndices = { _param->topoIndices.module, { list.targetEnabledSource, _param->topoIndices.param.slot } };
    int runtimeSourceIndex = topo->audio.ParamAtTopo(sourceIndices)->runtimeParamIndex;
    double runtimeSourceNormalized = _plugGUI->HostContext()->GetAudioParamNormalized(runtimeSourceIndex);
    auto const& sourceModule = topo->static_->modules[sourceIndices.module.index];
    auto const& sourceParam = sourceModule.params[sourceIndices.param.index];
    int runtimeSourcePlain = sourceParam.List().NormalizedToPlainFast((float)runtimeSourceNormalized);
    EnableItems(getRootMenu(), runtimeSourcePlain);
  }
  ComboBox::showPopup();
}