#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBTopLevelEditor.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

using namespace juce;

FBPlugGUI::
~FBPlugGUI()
{
  for (auto& tle : _topLevelEditors)
  {
    tle.second.detail.dialog->setVisible(false);
    delete tle.second.detail.dialog;
    tle.second.detail.dialog = nullptr;
  }
}

FBPlugGUI::
FBPlugGUI(FBHostGUIContext* hostContext) :
_hostContext(hostContext)
{
  _tooltipWindow = StoreComponent<TooltipWindow>();
  addAndMakeVisible(_tooltipWindow);
  addMouseListener(this, true);
}

void
FBPlugGUI::SetScale(double scale)
{
  _scale = scale;
  setTransform(AffineTransform::scale(static_cast<float>(scale)));
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
FBPlugGUI::AudioParamNormalizedChangedFromUI(int index, double value)
{
  AudioParamNormalizedChanged(index);
  for (int i = 0; i < _paramListeners.size(); i++)
    _paramListeners[i]->AudioParamChanged(index, value, true);
}

void
FBPlugGUI::AddParamListener(IFBParamListener* listener)
{
  auto iter = std::find(_paramListeners.begin(), _paramListeners.end(), listener);
  FB_ASSERT(iter == _paramListeners.end());
  _paramListeners.push_back(listener);
}

void
FBPlugGUI::RemoveParamListener(IFBParamListener* listener)
{
  auto iter = std::find(_paramListeners.begin(), _paramListeners.end(), listener);
  FB_ASSERT(iter != _paramListeners.end());
  _paramListeners.erase(iter);
}

void
FBPlugGUI::AudioParamNormalizedChangedFromHost(int index, double value)
{
  auto controlCount = GetControlCountForAudioParamIndex(index);
  for (int i = 0; i < controlCount; i++)
  {
    auto control = GetControlForAudioParamIndex(index, i);
    control->SetValueNormalizedFromHost(value);
  }
  AudioParamNormalizedChanged(index);
  for (int i = 0; i < _paramListeners.size(); i++)
    _paramListeners[i]->AudioParamChanged(index, value, false);
}

FBGUIParamControl* 
FBPlugGUI::GetControlForGUIParamIndex(int paramIndex) const
{
  auto guiControl = _store[_guiParamIndexToComponent.at(paramIndex)].get();
  return &dynamic_cast<FBGUIParamControl&>(*guiControl);
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
FBPlugGUI::GUIParamNormalizedChanged(int index, double value)
{
  auto iter = _guiParamIndexToComponent.find(index);
  if (iter != _guiParamIndexToComponent.end())
    GetControlForGUIParamIndex(index)->SetValueNormalizedFromPlug(value);
  GUIParamNormalizedChanged(index);
}

void
FBPlugGUI::AudioParamNormalizedChanged(int index)
{
  auto const& paramTopo = HostContext()->Topo()->audio.params[index].static_;
  if (paramTopo.mode == FBParamMode::Output || !paramTopo.NonRealTime().IsStepped())
    return;
  for (auto target : _audioParamsVisibleDependents[index])
    target->DependenciesChanged(true);
  for (auto target : _audioParamsEnabledDependents[index])
    target->DependenciesChanged(false);
}

int 
FBPlugGUI::GetControlCountForAudioParamIndex(int paramIndex) const
{
  auto iter = _audioParamIndexToComponents.find(paramIndex);
  if (iter == _audioParamIndexToComponents.end())
  {
    // Allow opt-out of otherwise obligatory minimum of 1 controller for param.
    auto const& paramTopo = HostContext()->Topo()->audio.params[paramIndex];
    if (paramTopo.static_.mode == FBParamMode::Fake)
      return 0;
  }
#ifndef NDEBUG
  if (iter == _audioParamIndexToComponents.end())
  {
    auto id = this->HostContext()->Topo()->audio.params[paramIndex].id;
    (void)id;
    FB_ASSERT(iter != _audioParamIndexToComponents.end());
  }
#endif
  return (int)iter->second.size();
}

FBParamControl*
FBPlugGUI::GetControlForAudioParamIndex(int paramIndex, int controlIndex) const
{
  int count = GetControlCountForAudioParamIndex(paramIndex);
  FB_ASSERT(0 <= controlIndex && controlIndex < count);
  return &dynamic_cast<FBParamControl&>(*_store[_audioParamIndexToComponents.at(paramIndex)[controlIndex]].get());
}

void
FBPlugGUI::UpdateExchangeStateTick()
{
  auto const& params = HostContext()->Topo()->audio.params;
  for (int i = 0; i < params.size(); i++)
    if (!params[i].static_.NonRealTime().IsStepped())
    {
      auto controlCount = GetControlCountForAudioParamIndex(i);
      for(int j = 0; j < controlCount; j++)
        dynamic_cast<FBParamSlider&>(*GetControlForAudioParamIndex(i, j)).UpdateExchangeState();
    }
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
  FB_LOG_ENTRY_EXIT();
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
    _audioParamIndexToComponents[audioParamControl->Param()->runtimeParamIndex].push_back(componentIndex);
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
  auto paramActive = HostContext()->ExchangeFromDSPState()->GetParamActiveState(&param);
  double engineMin = paramActive.active ? paramActive.minValue : normalized;
  double engineMax = paramActive.active ? paramActive.maxValue : normalized;

  std::string result = param.shortName + ": ";
  result += param.NormalizedToTextWithUnit(false, normalized);
  result += "\r\nEdit: " + FBEditTypeToString(param.static_.NonRealTime().GUIEditType());
  if (param.static_.mode == FBParamMode::Accurate || param.static_.mode == FBParamMode::VoiceStart)
    result += "\r\nAutomate: " + FBEditTypeToString(param.static_.NonRealTime().AutomationEditType());
  else
    result += "\r\nAutomate: None";
  if (param.static_.mode == FBParamMode::Accurate)
    result += "\r\nAutomation: Per-Sample";
  if (param.static_.mode == FBParamMode::VoiceStart)
    result += "\r\nAutomation: At Voice Start";
  if (!param.static_.IsVoice())
    result += "\r\nEngine: " + param.NormalizedToTextWithUnit(false, engineMin);
  else
  {
    result += "\r\nEngine min: " + param.NormalizedToTextWithUnit(false, engineMin);
    result += "\r\nEngine max: " + param.NormalizedToTextWithUnit(false, engineMax);
  }
#ifndef NDEBUG
  result += "\r\nParam index: " + std::to_string(index);
  result += "\r\nParam tag: " + std::to_string(param.tag);
#endif
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
  OnPatchChanged();
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
      OnPatchChanged();
    }
    else
      AlertWindow::showMessageBoxAsync(
        MessageBoxIconType::WarningIcon,
        "Error",
        "Failed to load patch. See log for details: " + FBGetLogPath(HostContext()->Topo()->static_->meta).string() + ".");
  });
}

void
FBPlugGUI::CloseTopLevelEditor(int id)
{
  FB_LOG_ENTRY_EXIT();
  auto iter = _topLevelEditors.find(id);
  if (iter != _topLevelEditors.end())
  {
    auto& params = _topLevelEditors.at(id);
    params.detail.dialog->setVisible(false);
    delete params.detail.dialog;
    params.detail.dialog = nullptr;

    int showParamIndex = HostContext()->Topo()->gui.ParamAtTopo({ { params.toggleModuleIndex, 0 }, { params.toggleParamIndex, 0 } })->runtimeParamIndex;
    HostContext()->SetGUIParamNormalized(showParamIndex, 0.0);
    GUIParamNormalizedChanged(showParamIndex, 0.0);
  }
}

void
FBPlugGUI::OpenTopLevelEditor(int id, FBTopLevelEditorParams const& params0)
{
  FB_LOG_ENTRY_EXIT();
  auto iter = _topLevelEditors.find(id);
  if (iter == _topLevelEditors.end())
    _topLevelEditors[id] = params0;
  auto& params = _topLevelEditors.at(id);

  // Don't check the dialog, that one gets deleted.
  // All the rest -- init once and keep in memory.
  bool firstInit = params.detail.contentComponent == nullptr;

  if (firstInit)
  {
    auto upperGrid = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 0, 0, 1 });
    upperGrid->Add(0, 0, params.header);
    auto lastTweakedGrid = StoreComponent<FBGridComponent>(true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 0, 0 });
    lastTweakedGrid->Add(0, 0, StoreComponent<FBLastTweakedLabel>(this));
    lastTweakedGrid->Add(0, 1, StoreComponent<FBLastTweakedTextBox>(this, 80));
    lastTweakedGrid->MarkSection({ { 0, 0 }, { 1, 2 } });
    auto lastTweakedSubSection = StoreComponent<FBSubSectionComponent>(lastTweakedGrid);
    auto lastTweakedSection = StoreComponent<FBSectionComponent>(lastTweakedSubSection);
    upperGrid->Add(0, 1, lastTweakedSection);
    auto initGrid = StoreComponent<FBGridComponent>(true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 1, 0 });
    auto initButton = StoreComponent<FBAutoSizeButton>("Init");
    initButton->onClick = params.init;
    initGrid->Add(0, 0, StoreComponent<FBFillerComponent>(1, 1));
    initGrid->Add(0, 1, initButton);
    initGrid->MarkSection({ { 0, 0 }, { 1, 2 } });
    auto initSubSection = StoreComponent<FBSubSectionComponent>(initGrid);
    auto initSection = StoreComponent<FBSectionComponent>(initSubSection);
    upperGrid->Add(0, 2, initSection);

    auto grid = StoreComponent<FBGridComponent>(false, -1, -1, std::vector<int> { 0, 1 }, std::vector<int> { 1 });
    grid->Add(0, 0, upperGrid);
    auto contentSubSection = StoreComponent<FBSubSectionComponent>(params.content);
    auto contentSection = StoreComponent<FBSectionComponent>(contentSubSection);
    grid->Add(1, 0, contentSection);
    grid->addAndMakeVisible(StoreComponent<TooltipWindow>());
    params.detail.contentGrid = grid;
  }

  params.detail.contentGrid->setTransform(AffineTransform::scale(static_cast<float>(_scale)));
  params.detail.contentGrid->setBounds(0, 0, params.w, params.h);
  params.detail.contentGrid->resized();

  if (firstInit)
  {
    auto* content = StoreComponent<Component>();
    params.detail.contentComponent = content;
    content->addAndMakeVisible(params.detail.contentGrid);
  }

  params.detail.contentComponent->setBounds(0, 0, (int)std::round(params.w * _scale), (int)std::round(params.h * _scale));

  if (params.detail.dialog == nullptr)
  {
    juce::DialogWindow::LaunchOptions options;
    options.resizable = false;
    options.useNativeTitleBar = true;
    options.componentToCentreAround = this;
    options.escapeKeyTriggersCloseButton = true;
    options.dialogBackgroundColour = Colours::black;
    options.content = OptionalScopedPointer<Component>(params.detail.contentComponent, false);
    options.dialogTitle = HostContext()->Topo()->static_->meta.shortName + " " + params.title;

    auto iconPath = (FBGUIGetResourcesFolderPath() / params.iconFile).string();
    auto icon = ImageCache::getFromFile(File(iconPath));
    params.detail.dialog = new FBTopLevelEditor(this, id, options);
    params.detail.dialog->setOpaque(true);
    params.detail.dialog->setUsingNativeTitleBar(true);
    params.detail.dialog->setResizable(false, false);
    params.detail.dialog->setIcon(icon);
    params.detail.dialog->getPeer()->setIcon(icon);
  }

  int showParamIndex = HostContext()->Topo()->gui.ParamAtTopo({ { params.toggleModuleIndex, 0 }, { params.toggleParamIndex, 0 } })->runtimeParamIndex;
  if (firstInit)
  {
    auto guiControl = GetControlForGUIParamIndex(showParamIndex);
    auto guiToggle = &dynamic_cast<FBGUIParamToggleButton&>(*guiControl);
    guiToggle->onClick = [this, id, guiToggle, params0]() {
      if (guiToggle->getToggleState())
        OpenTopLevelEditor(id, params0);
      else
        CloseTopLevelEditor(id);
    };
  }

  HostContext()->SetGUIParamNormalized(showParamIndex, 1.0);
  GUIParamNormalizedChanged(showParamIndex, 1.0);

  params.detail.dialog->setVisible(true);
  params.detail.dialog->grabKeyboardFocus();
}