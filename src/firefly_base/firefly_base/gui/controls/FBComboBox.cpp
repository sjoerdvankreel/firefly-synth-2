#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

class FBComboMenuCloseListener:
public PopupMenu::CustomCallback
{
  int const _itemResultId;
  FBAutoSizeComboBox* const _box;
public:
  bool menuItemTriggered() override;
  FBComboMenuCloseListener(FBAutoSizeComboBox* box, int itemResultId) : 
  _itemResultId(itemResultId), _box(box) {}
};

bool 
FBComboMenuCloseListener::menuItemTriggered()
{
  _box->OnPopupMenuClosing(_itemResultId);
  return true;
}

FBAutoSizeComboBox::
FBAutoSizeComboBox(PopupMenu const& rootMenu):
ComboBox()
{
  *getRootMenu() = rootMenu;
  PopupMenu::MenuItemIterator iter(*getRootMenu(), true);
  while (iter.next())
    iter.getItem().customCallback = new FBComboMenuCloseListener(this, iter.getItem().itemID);
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
FBParamComboBox::showPopup()
{
  // need to catch real user input for the undo state, not all kinds of async callbacks
  // this will cause some spurious undo items if user opens popup but not changes it
  _plugGUI->HostContext()->UndoState().Snapshot("Change " + _param->longName);

  // enable/disable items based on the source combo
  if (_param->static_.type == FBParamType::List && 
    _param->static_.List().linkedTargetEnabledSelector != nullptr)
    UpdateDependentComboboxTarget();

  ComboBox::showPopup();
}

void 
FBParamComboBox::EnableDependentItems(PopupMenu* menu, int runtimeSourceValue)
{
  PopupMenu::MenuItemIterator iter(*menu);
  while (iter.next())
  {
    if (iter.getItem().subMenu != nullptr)
      EnableDependentItems(iter.getItem().subMenu.get(), runtimeSourceValue);
    else
      iter.getItem().isEnabled = _param->static_.List().linkedTargetEnabledSelector(
        runtimeSourceValue, iter.getItem().itemID - 1);
  }
}

void 
FBParamComboBox::UpdateDependentComboboxTarget()
{
  auto const& list = _param->static_.List();
  auto const* topo = _plugGUI->HostContext()->Topo();
  FB_ASSERT(list.linkedSource != -1);
  FBParamTopoIndices sourceIndices = { _param->topoIndices.module, { list.linkedSource, _param->topoIndices.param.slot } };
  int runtimeSourceIndex = topo->audio.ParamAtTopo(sourceIndices)->runtimeParamIndex;
  FB_ASSERT(topo->audio.params[runtimeSourceIndex].static_.List().linkedTarget != -1);
  double runtimeSourceNormalized = _plugGUI->HostContext()->GetAudioParamNormalized(runtimeSourceIndex);
  auto const& sourceModule = topo->static_->modules[sourceIndices.module.index];
  auto const& sourceParam = sourceModule.params[sourceIndices.param.index];
  int runtimeSourcePlain = sourceParam.List().NormalizedToPlainFast((float)runtimeSourceNormalized);
  EnableDependentItems(getRootMenu(), runtimeSourcePlain);
}

void
FBParamComboBox::OnPopupMenuClosing(int itemResultId)
{
  if (_param->static_.type != FBParamType::List)
    return;
  auto const& list = _param->static_.List();
  if (list.linkedTarget == -1)
    return;

  // If we're about to close the source of a dependent combo, 
  // set target to 0 (assumed to be default) if it's not allowed.
  auto const* topo = _plugGUI->HostContext()->Topo();
  FBParamTopoIndices targetIndices = { _param->topoIndices.module, { list.linkedTarget, _param->topoIndices.param.slot } };
  int runtimeTargetIndex = topo->audio.ParamAtTopo(targetIndices)->runtimeParamIndex;
  double runtimeTargetNormalized = _plugGUI->HostContext()->GetAudioParamNormalized(runtimeTargetIndex);
  auto const& targetModule = topo->static_->modules[targetIndices.module.index];
  auto const& targetParam = targetModule.params[targetIndices.param.index];
  int runtimeTargetPlain = targetParam.List().NormalizedToPlainFast((float)runtimeTargetNormalized);
  bool targetIsAllowed = targetParam.List().linkedTargetEnabledSelector(itemResultId - 1, runtimeTargetPlain);
  if (!targetIsAllowed)
    MessageManager::callAsync([this, runtimeTargetIndex] { _plugGUI->HostContext()->PerformImmediateAudioParamEdit(runtimeTargetIndex, 0.0); });
}