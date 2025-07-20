#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

using namespace juce;

FBPlugGUI::
FBPlugGUI(FBHostGUIContext* hostContext) :
_hostContext(hostContext)
{
  _tooltipWindow = StoreComponent<TooltipWindow>();
  addAndMakeVisible(_tooltipWindow);
  addMouseListener(this, true);
}

void
FBPlugGUI::InitAllDependencies()
{
  FB_LOG_ENTRY_EXIT();

  auto const& guiParams = HostContext()->Topo()->gui.params;
  for (int i = 0; i < guiParams.size(); i++)
    GUIParamNormalizedChanged(i);

  auto const& audioParams = HostContext()->Topo()->audio.params;
  for (int i = 0; i < audioParams.size(); i++)
    AudioParamNormalizedChanged(i);
}

void
FBPlugGUI::GUIParamNormalizedChanged(int index, double /*value*/)
{
  GUIParamNormalizedChanged(index);
}

void
FBPlugGUI::AudioParamNormalizedChangedFromUI(int index, double /*value*/)
{
  AudioParamNormalizedChanged(index);
}

void
FBPlugGUI::AudioParamNormalizedChangedFromHost(int index, double value)
{
  auto control = GetControlForAudioParamIndex(index);
  control->SetValueNormalizedFromHost(value);
  AudioParamNormalizedChanged(index);
}

void
FBPlugGUI::GUIParamNormalizedChanged(int index)
{
  auto const& params = HostContext()->Topo()->gui.params;
  if (!params[index].static_.NonRealTime().IsStepped())
    return;
  for (auto target : _guiParamsVisibleDependents[index])
    target->DependenciesChanged(true);
  for (auto target : _guiParamsEnabledDependents[index])
    target->DependenciesChanged(false);
}

void
FBPlugGUI::AudioParamNormalizedChanged(int index)
{
  auto const& paramTopo = HostContext()->Topo()->audio.params[index].static_;
  if (paramTopo.output || !paramTopo.NonRealTime().IsStepped())
    return;
  for (auto target : _audioParamsVisibleDependents[index])
    target->DependenciesChanged(true);
  for (auto target : _audioParamsEnabledDependents[index])
    target->DependenciesChanged(false);
}

FBParamControl*
FBPlugGUI::GetControlForAudioParamIndex(int paramIndex) const
{
  auto iter = _audioParamIndexToComponent.find(paramIndex);
#ifndef NDEBUG
  if (iter == _audioParamIndexToComponent.end())
  {
    auto id = this->HostContext()->Topo()->audio.params[paramIndex].id;
    (void)id;
    FB_ASSERT(iter != _audioParamIndexToComponent.end());
  }
#endif
  return &dynamic_cast<FBParamControl&>(*_store[iter->second].get());
}

void
FBPlugGUI::UpdateExchangeStateTick()
{
  auto const& params = HostContext()->Topo()->audio.params;
  for (int i = 0; i < params.size(); i++)
    if (!params[i].static_.NonRealTime().IsStepped())
      dynamic_cast<FBParamSlider&>(*GetControlForAudioParamIndex(i)).UpdateExchangeState();
}

void
FBPlugGUI::ShowPopupMenuFor(
  Component* target,
  PopupMenu menu,
  std::function<void(int)> callback)
{
  PopupMenu::Options options;
  options = options.withParentComponent(this);
  options = options.withTargetComponent(target);
  options = options.withMousePosition();
  menu.showMenuAsync(options, callback);
}

void
FBPlugGUI::UpdateExchangeState()
{
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;

  auto now = high_resolution_clock::now();
  auto elapsedMillis = duration_cast<milliseconds>(now - _exchangeUpdated);
  if (elapsedMillis.count() < 1000.0 / FBGUIFPS)
    return;
  _exchangeUpdated = now;
  UpdateExchangeStateTick();
}

void
FBPlugGUI::ShowHostMenuForAudioParam(int index)
{
  auto menuItems = HostContext()->MakeAudioParamContextMenu(index);
  if (menuItems.empty())
    return;
  auto hostMenu = FBMakeHostContextMenu(menuItems);
  auto clicked = [this, index](int tag) {
    if (tag > 0)
      HostContext()->AudioParamContextMenuClicked(index, tag); };
  ShowPopupMenuFor(this, *hostMenu, clicked);
}

Component*
FBPlugGUI::StoreComponent(std::unique_ptr<Component>&& component)
{
  FBParamControl* audioParamControl;
  FBGUIParamControl* guiParamControl;
  FBParamsDependent* paramsDependent;
  Component* result = component.get();
  int componentIndex = (int)_store.size();
  _store.emplace_back(std::move(component));
  if ((audioParamControl = dynamic_cast<FBParamControl*>(result)) != nullptr)
    _audioParamIndexToComponent[audioParamControl->Param()->runtimeParamIndex] = componentIndex;
  if ((guiParamControl = dynamic_cast<FBGUIParamControl*>(result)) != nullptr)
    _guiParamIndexToComponent[guiParamControl->Param()->runtimeParamIndex] = componentIndex;
  if ((paramsDependent = dynamic_cast<FBParamsDependent*>(result)) != nullptr)
  {
    for (int p : paramsDependent->RuntimeDependencies(false, true))
      _guiParamsVisibleDependents[p].insert(paramsDependent);
    for (int p : paramsDependent->RuntimeDependencies(false, false))
      _guiParamsEnabledDependents[p].insert(paramsDependent);
    for (int p : paramsDependent->RuntimeDependencies(true, true))
      _audioParamsVisibleDependents[p].insert(paramsDependent);
    for (int p : paramsDependent->RuntimeDependencies(true, false))
      _audioParamsEnabledDependents[p].insert(paramsDependent);
  }
  return result;
}

std::string 
FBPlugGUI::GetTooltipForGUIParam(int index) const
{
  auto const& param = HostContext()->Topo()->gui.params[index];
  double normalized = HostContext()->GetGUIParamNormalized(index);
  std::string result = param.shortName + ": " + param.NormalizedToTextWithUnit(false, normalized);
  result += "\r\nEdit: " + FBEditTypeToString(param.static_.NonRealTime().GUIEditType());
  return result;
}

std::string
FBPlugGUI::GetTooltipForAudioParam(int index) const
{
  auto const& param = HostContext()->Topo()->audio.params[index];
  double normalized = HostContext()->GetAudioParamNormalized(index);
  auto paramActive = HostContext()->ExchangeState()->GetParamActiveState(&param);
  double engineMin = paramActive.active ? paramActive.minValue : normalized;
  double engineMax = paramActive.active ? paramActive.maxValue : normalized;

  auto result = param.shortName + ": ";
  result += param.NormalizedToTextWithUnit(false, normalized);  
  if (!param.static_.IsVoice())
    result += "\r\nEngine: " + param.NormalizedToTextWithUnit(false, engineMin);
  else
  {
    result += "\r\nEngine min: " + param.NormalizedToTextWithUnit(false, engineMin);
    result += "\r\nEngine max: " + param.NormalizedToTextWithUnit(false, engineMax);
  }
  result += "\r\nEdit: " + FBEditTypeToString(param.static_.NonRealTime().GUIEditType());
  if (param.static_.AutomationTiming() != FBAutomationTiming::Never)
    result += "\r\nAutomate: " + FBEditTypeToString(param.static_.NonRealTime().AutomationEditType());
  else
    result += "\r\nAutomate: None";
  result += "\r\nAutomation: " + FBAutomationTimingToString(param.static_.AutomationTiming());
  return result;
}

void 
FBPlugGUI::mouseUp(const MouseEvent& event)
{
  if (!event.mods.isRightButtonDown())
  {
    Component::mouseUp(event);
    return;
  }

  // pops up host context menu
  if (dynamic_cast<FBParamControl*>(event.eventComponent))
    return;

  // pops up module context menu
  if (dynamic_cast<FBTabBarButton*>(event.eventComponent))
    return;

  auto& undoState = HostContext()->UndoState();
  if (!undoState.CanUndo() && !undoState.CanRedo())
    return;

  PopupMenu menu;
  if (undoState.CanUndo())
    menu.addItem(1, "Undo " + undoState.UndoAction());
  if (undoState.CanRedo())
    menu.addItem(2, "Redo " + undoState.RedoAction());
  PopupMenu::Options options;
  options = options.withParentComponent(this);
  options = options.withMousePosition();
  menu.showMenuAsync(options, [this](int id) {
    if (id == 1) HostContext()->UndoState().Undo();
    if(id == 2) HostContext()->UndoState().Redo(); });
}

void 
FBPlugGUI::InitPatch()
{
  FB_LOG_ENTRY_EXIT();
  HostContext()->UndoState().Snapshot("Init Patch");
  FBScalarStateContainer defaultState(*HostContext()->Topo());
  for (int i = 0; i < defaultState.Params().size(); i++)
    HostContext()->PerformImmediateAudioParamEdit(i, *defaultState.Params()[i]);
}

void 
FBPlugGUI::SavePatchToFile()
{
  FB_LOG_ENTRY_EXIT();
  int saveFlags = FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting;
  auto extension = HostContext()->Topo()->static_->patchExtension;
  FileChooser* chooser = new FileChooser("Save Patch", File(), String("*.") + extension, true, false, this);
  chooser->launchAsync(saveFlags, [this](FileChooser const& chooser) {
    FB_LOG_ENTRY_EXIT();
    auto file = chooser.getResult();
    delete &chooser;
    if (file.getFullPathName().length() == 0) return;
    FBScalarStateContainer editState(*HostContext()->Topo());
    editState.CopyFrom(HostContext());
    file.replaceWithText(HostContext()->Topo()->SaveEditStateToString(editState));
  });
}

void 
FBPlugGUI::LoadPatchFromFile()
{
  FB_LOG_ENTRY_EXIT();
  int loadFlags = FileBrowserComponent::openMode;
  auto extension = HostContext()->Topo()->static_->patchExtension;
  FileChooser* chooser = new FileChooser("Load Patch", File(), String("*.") + extension, true, false, this);
  chooser->launchAsync(loadFlags, [this](FileChooser const& chooser) {
    auto file = chooser.getResult();
    delete &chooser;
    if (file.getFullPathName().length() == 0) return;
    auto text = file.loadFileAsString().toStdString();
    FBScalarStateContainer editState(*HostContext()->Topo());
    if (HostContext()->Topo()->LoadEditStateFromString(text, editState))
    {
      HostContext()->UndoState().Snapshot("Load Patch");
      editState.CopyTo(HostContext());
    }
    else
      AlertWindow::showMessageBoxAsync(
        MessageBoxIconType::WarningIcon,
        "Error",
        "Failed to load patch. See log for details.\r\n" + FBGetLogPath(HostContext()->Topo()->static_->meta).string());
  });
}