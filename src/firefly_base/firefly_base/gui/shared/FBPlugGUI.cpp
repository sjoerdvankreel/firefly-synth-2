#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUIListeners.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBParamNameEditor.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBImageComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBContentComponent.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

using namespace juce;

FBPlugGUI::
~FBPlugGUI()
{
  _hostContext->RemoveListener(this);
}

FBPlugGUI::
FBPlugGUI(FBHostGUIContext* hostContext) :
_hostContext(hostContext),
_lookAndFeel(std::make_unique<FBLookAndFeel>()),
_patchBeforePreview(*hostContext->Topo())
{
  _themes = FBLoadThemes(hostContext->Topo());
  if (_themes.empty())
    FB_LOG_ERROR("No themes found.");
  setLookAndFeel(_lookAndFeel.get());
  _tooltipWindow = StoreComponent<TooltipWindow>();
  _hostContext->AddListener(this);

  InitLoadPatchBrowser(false);
  InitSavePatchBrowser();
  InitDumpTopologyBrowser();
  InitDumpParamListBrowser();

  addAndMakeVisible(_tooltipWindow);
  addMouseListener(this, true);
  SetupOverlayGUI();
  SetupAboutBoxGUI();
  _paramNameEditor = StoreComponent<FBParamNameEditor>(this, 240);
}

void
FBPlugGUI::SetBrowserInitialPath(std::string const& browserKey, File const& selected)
{
  auto dir = selected.getParentDirectory().getFullPathName().toStdString();
  FBSetUserGlobalSetting(HostContext()->Topo()->static_->meta, browserKey, dir);
}

File 
FBPlugGUI::GetBrowserInitialPath(std::string const& browserKey)
{
  std::string initialPathString;
  if (!FBGetUserGlobalSetting(HostContext()->Topo()->static_->meta, browserKey, initialPathString))
    return File();
  File result(initialPathString);
  if (!result.exists())
    return File();
  return result;
}

void 
FBPlugGUI::InitLoadPatchBrowser(bool preset)
{
  auto extension = HostContext()->Topo()->static_->patchExtension;
  auto filterName = HostContext()->Topo()->static_->patchFilterName;
  auto initialPath = GetBrowserInitialPath(FBUserGlobalSettingKeys::LoadPatchFolder);
  if (preset)
  {
    initialPath = File((FBGetPresetsFolderPath() / (HostContext()->Topo()->static_->meta.isFx? "Fx": "Instrument")).string());
  }
  _loadPatchBrowser = std::make_unique<FBFileBrowserComponent>(this, false, true, "Load Patch", extension, filterName, initialPath, [this, preset](juce::File const& file) {
    _isPatchPreviewEnabled = _loadPatchBrowser->IsPreviewEnabled();
    if(!preset)
      SetBrowserInitialPath(FBUserGlobalSettingKeys::LoadPatchFolder, file);
    auto text = file.loadFileAsString().toStdString();
    if (!LoadPatchFromText("Load Patch", file.getFileNameWithoutExtension().toStdString(), text))
      AlertWindow::showMessageBoxAsync(
        MessageBoxIconType::NoIcon,
        "Error",
        "Failed to load patch. See log for details.");
    }, [this](juce::File const& file) { 
      LoadPatchAsPreview(file); 
    }, [this] {
      RevertPreviewedPatch();
    });
  _loadPatchBrowser->SetPreviewEnabled(_isPatchPreviewEnabled);
}

void 
FBPlugGUI::InitSavePatchBrowser()
{
  auto extension = HostContext()->Topo()->static_->patchExtension;
  auto filterName = HostContext()->Topo()->static_->patchFilterName;
  auto initialPath = GetBrowserInitialPath(FBUserGlobalSettingKeys::SavePatchFolder);
  _savePatchBrowser = std::make_unique<FBFileBrowserComponent>(this, true, false, "Save Patch", extension, filterName, initialPath, [this](juce::File const& file) {
    SetBrowserInitialPath(FBUserGlobalSettingKeys::SavePatchFolder, file);
    FBScalarStateContainer editState(*HostContext()->Topo());
    editState.CopyFrom(HostContext(), true);
    file.replaceWithText(HostContext()->Topo()->SavePatchStateToString(editState, *HostContext()));
  }, [](juce::File const&){}, [](){});
}

void 
FBPlugGUI::InitDumpTopologyBrowser()
{
  auto initialPath = GetBrowserInitialPath(FBUserGlobalSettingKeys::SaveTopologyFolder);
  _saveTopologyBrowser = std::make_unique<FBFileBrowserComponent>(this, true, false, "Dump Topology", "txt", "Text Files", initialPath, [this](juce::File const& file) {
    SetBrowserInitialPath(FBUserGlobalSettingKeys::SaveTopologyFolder, file);
    file.replaceWithText(HostContext()->Topo()->static_->PrintTopology());
  }, [](juce::File const&){}, []() {});
}

void 
FBPlugGUI::InitDumpParamListBrowser()
{
  auto initialPath = GetBrowserInitialPath(FBUserGlobalSettingKeys::SaveParamListFolder);
  _saveParamListBrowser = std::make_unique<FBFileBrowserComponent>(this, true, false, "Dump Param List", "txt", "Text Files", initialPath, [this](juce::File const& file) {
    SetBrowserInitialPath(FBUserGlobalSettingKeys::SaveParamListFolder, file);
    file.replaceWithText(HostContext()->Topo()->PrintParamList());
  }, [](juce::File const&){}, []() {});
}

FBTheme const& 
FBPlugGUI::GetTheme() const
{
  static FBTheme fallback = {};
  for (int i = 0; i < Themes().size(); i++)
    if (Themes()[i].global.name == HostContext()->ThemeName())
      return Themes()[i];
  if (Themes().size() > 0)
    return Themes()[0];
  return fallback;
}

void 
FBPlugGUI::SwitchTheme(std::string const& themeName)
{
  HostContext()->SetThemeName(themeName);
  FBGetLookAndFeelFor(this)->SetTheme(GetTheme());
  for (int i = 0; i < _themeListeners.size(); i++)
    _themeListeners[i]->ThemeChanged();
  ForceReLayout(); // font size depends on theme
}

void
FBPlugGUI::AfterPatchChanged(bool wasPreview)
{
  if(!wasPreview)
    RequestGUIReset();
}

void 
FBPlugGUI::RequestGUIReset()
{
  for (int i = 0; i < _resetListeners.size(); i++)
    _resetListeners[i]->OnResetRequest();
}

void
FBPlugGUI::SetScale(double scale)
{
  _scale = scale;
  setTransform(AffineTransform::scale(static_cast<float>(scale)));
}

void
FBPlugGUI::HideTooltip()
{
  _tooltipWindow->hideTip();
}

void 
FBPlugGUI::SetTooltip(Point<int> const& screenPos, String const& text)
{
  _tooltipWindow->displayTip(screenPos, text);
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
FBPlugGUI::FlashAudioParamDisabling(int index)
{
  int controlCount = GetControlCountForAudioParamIndex(index);
  for (int i = 0; i < controlCount; i++)
    GetControlForAudioParamIndex(index, i)->StartFlashDisabling();
}

void
FBPlugGUI::FlashAudioParamsDisablingParam(int index)
{
  int controlCount = GetControlCountForAudioParamIndex(index);
  for (int i = 0; i < controlCount; i++)
  {
    auto const* control = GetControlForAudioParamIndex(index, i);
    auto dependencies = GetAudioParamEnabledDependenciesExcludingSelf(control);
    for (int j = 0; j < dependencies.size(); j++)
      FlashAudioParamDisabling(dependencies[j]);
  }
}

std::vector<int> 
FBPlugGUI::GetAudioParamEnabledDependenciesExcludingSelf(
  FBParamControl const* control) const
{
  std::vector<int> result = {};
  int index = control->Param()->runtimeParamIndex;
  auto const& dependencies = control->RuntimeDependencies(true, false);
  std::copy_if(dependencies.begin(), dependencies.end(), std::back_inserter(result), [index](auto const& e) { return e != index;  });
  return result;
}

void
FBPlugGUI::AudioParamNormalizedChangedFromUI(int index, double value)
{
  AudioParamNormalizedChanged(index);
  for (int i = 0; i < _paramListeners.size(); i++)
    _paramListeners[i]->AudioParamChanged(index, value, true);
}

void
FBPlugGUI::AddGUIResetListener(IFBGUIResetListener* listener)
{
  auto iter = std::find(_resetListeners.begin(), _resetListeners.end(), listener);
  FB_ASSERT(iter == _resetListeners.end());
  _resetListeners.push_back(listener);
}

void
FBPlugGUI::RemoveGUIResetListener(IFBGUIResetListener* listener)
{
  auto iter = std::find(_resetListeners.begin(), _resetListeners.end(), listener);
  FB_ASSERT(iter != _resetListeners.end());
  _resetListeners.erase(iter);
}

void
FBPlugGUI::AddThemeListener(IFBThemeListener* listener)
{
  auto iter = std::find(_themeListeners.begin(), _themeListeners.end(), listener);
  FB_ASSERT(iter == _themeListeners.end());
  _themeListeners.push_back(listener);
}

void
FBPlugGUI::RemoveThemeListener(IFBThemeListener* listener)
{
  auto iter = std::find(_themeListeners.begin(), _themeListeners.end(), listener);
  FB_ASSERT(iter != _themeListeners.end());
  _themeListeners.erase(iter);
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
FBPlugGUI::SetAudioParamNameOverride(int index)
{
  _paramNameEditor->InitEdit(index);
  auto const& runtimeParam = HostContext()->Topo()->audio.params[index];
  auto const& indices = HostContext()->Topo()->modules[runtimeParam.runtimeModuleIndex].topoIndices;
  ShowOverlayComponent("Edit Param Name", indices.index, indices.slot, _paramNameEditor, 240, 70, true, 
    [this, index]() { _paramNameEditor->ClearEdit(index); },
    [this, index]() {
      HostContext()->UndoState().Snapshot("Change Param Name");
      auto text = _paramNameEditor->getText().trim();
      if (text.isEmpty())
        HostContext()->ClearAudioParamNameOverride(index);
      else
        HostContext()->SetAudioParamNameOverride(index, text.toStdString());
      HostContext()->NotifyHostOfParamNameChanges();
    });
}

void
FBPlugGUI::ClearAudioParamNameOverride(int index)
{
  _hostContext->ClearAudioParamNameOverride(index);
  _hostContext->NotifyHostOfParamNameChanges();
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
    GetControlForGUIParamIndex(index)->SetValueNormalized(value);
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

void 
FBPlugGUI::RepaintSlidersForAudioParam(FBParamTopoIndices const& indices)
{
  int targetIndex = HostContext()->Topo()->audio.ParamAtTopo(indices)->runtimeParamIndex;
  int controlCount = GetControlCountForAudioParamIndex(targetIndex);
  for (int i = 0; i < controlCount; i++)
  {
    auto control = GetControlForAudioParamIndex(targetIndex, i);
    if(auto slider = dynamic_cast<FBParamSlider*>(control))
      slider->repaint();
  }
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
      for (int j = 0; j < controlCount; j++)
        if (auto ps = dynamic_cast<FBParamSlider*>(GetControlForAudioParamIndex(i, j)))
          ps->UpdateExchangeState();
    }
}

void
FBPlugGUI::ShowPopupMenuFor(
  Component* target,
  PopupMenu menu,
  std::function<void(int)> callback)
{
  PopupMenu::Options options;
  auto lnf = FBGetLookAndFeelFor(this);
  options = options.withParentComponent(this);
  options = options.withTargetComponent(target);
  options = options.withStandardItemHeight(lnf->GetStandardPopupMenuItemHeight());
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
FBPlugGUI::ShowMenuForGUIParam(int index)
{
  FB_LOG_ENTRY_EXIT();
  auto menu = std::make_shared<PopupMenu>();
  menu->addItem(1, "Show Manual");
  menu->addSeparator();
  menu->addItem(2, "Set To Default");
  auto clicked = [this, index](int tag) {
    if (tag <= 0)
      return;
    if (tag == 1)
    {
      HostContext()->ShowOnlineManualForGUIParam(index);
    }
    else if (tag == 2)
    {
      double normalized = HostContext()->Topo()->gui.params[index].DefaultNormalizedByText();
      HostContext()->SetGUIParamNormalized(index, normalized);
      GUIParamNormalizedChanged(index, normalized);
    }
  };
  auto* control = &dynamic_cast<Component&>(*GetControlForGUIParamIndex(index));
  ShowPopupMenuFor(control, *menu, clicked);
}

void
FBPlugGUI::ShowMenuForAudioParam(int index, bool showHostMenu)
{
  FB_LOG_ENTRY_EXIT();
  auto menu = std::make_shared<PopupMenu>();
  menu->addItem(1, "Show Manual");
  menu->addItem(2, "Rename Param");
  menu->addItem(3, "Clear Param Name");
  menu->addSeparator();
  menu->addItem(4, "Set To Patch");
  menu->addItem(5, "Set To Session");
  menu->addItem(6, "Set To Default");
  if (showHostMenu)
  {
    auto hostMenuItems = HostContext()->MakeAudioParamContextMenu(index);
    if (!hostMenuItems.empty())
    {
      menu->addSeparator();
      FBAddHostContextMenu(menu, 1000, hostMenuItems);
    }
  }
  auto clicked = [this, index](int tag) {
    if (tag <= 0)
      return;
    if (tag == 1)
    {
      HostContext()->ShowOnlineManualForAudioParam(index);
    }
    else if (tag == 2)
    {
      SetAudioParamNameOverride(index);
    }
    else if (tag == 3)
    {
      ClearAudioParamNameOverride(index);
    }
    else if (tag == 4)
    {
      HostContext()->UndoState().Snapshot("Set " + HostContext()->Topo()->audio.params[index].shortName + " To Patch");
      HostContext()->PerformImmediateAudioParamEdit(index, *HostContext()->PatchState().Params()[index]);
    }
    else if (tag == 5)
    {
      HostContext()->UndoState().Snapshot("Set " + HostContext()->Topo()->audio.params[index].shortName + " To Session");
      HostContext()->PerformImmediateAudioParamEdit(index, *HostContext()->SessionState().Params()[index]);
    }
    else if (tag == 6)
    {
      HostContext()->UndoState().Snapshot("Set " + HostContext()->Topo()->audio.params[index].shortName + " To Default");
      HostContext()->PerformImmediateAudioParamEdit(index, HostContext()->Topo()->audio.params[index].DefaultNormalizedByText());
    }
    else
    {
      HostContext()->AudioParamContextMenuClicked(index, tag - 1000);
    }
  };
  auto* control = &dynamic_cast<Component&>(*GetControlForAudioParamIndex(index, 0));
  ShowPopupMenuFor(control, *menu, clicked);
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
  std::string result = param.static_.description;
  result += "\r\n";
  result += "\r\n" + param.shortName + ": " + param.NormalizedToTextWithUnit(false, normalized);
  result += "\r\nEdit: " + FBEditTypeToString(param.static_.NonRealTime().GUIEditType());
  result += "\r\nStored In: Session Only";
  return result;
}

std::string
FBPlugGUI::GetTooltipForAudioParam(FBParamControl const* control) const
{
  int index = control->Param()->runtimeParamIndex;
  auto const& param = HostContext()->Topo()->audio.params[index];
  double normalized = HostContext()->GetAudioParamNormalized(index);
  auto paramActive = HostContext()->ExchangeFromDSPState()->GetParamActiveState(&param);
  double engineMin = paramActive.active ? paramActive.minValue : normalized;
  double engineMax = paramActive.active ? paramActive.maxValue : normalized;

  std::string overrideName;
  std::string result = param.static_.description;
  if (HostContext()->GetAudioParamNameOverride(index, overrideName))
    result += "\r\n" + overrideName;

#ifndef NDEBUG
  result += "\r\nParam index: " + std::to_string(index);
  result += "\r\nParam tag: " + std::to_string(param.tag);
#endif

  result += "\r\n";
  result += "\r\n" + param.shortName + ": ";
  result += param.NormalizedToTextWithUnit(false, normalized);
  if (param.static_.IsOutput())
    return result;

  if (!param.static_.IsVoice())
    result += "\r\nEngine: " + param.NormalizedToTextWithUnit(false, engineMin);
  else
  {
    result += "\r\nEngine min: " + param.NormalizedToTextWithUnit(false, engineMin);
    result += "\r\nEngine max: " + param.NormalizedToTextWithUnit(false, engineMax);
  }

  result += "\r\n";
  result += "\r\nEdit: " + FBEditTypeToString(param.static_.NonRealTime().GUIEditType());
  if (param.static_.mode == FBParamMode::Accurate || param.static_.mode == FBParamMode::VoiceStart)
    result += "\r\nAutomate: " + FBEditTypeToString(param.static_.NonRealTime().AutomationEditType());
  else
    result += "\r\nAutomate: None";
  if (param.static_.mode == FBParamMode::Accurate)
    result += "\r\nAutomation: Per-Sample";
  if (param.static_.mode == FBParamMode::VoiceStart)
    result += "\r\nAutomation: At Voice Start";

  auto controlComponent = &dynamic_cast<Component const&>(*control);
  if (!controlComponent->isEnabled())
  {
    std::vector<int> filteredDependencies = GetAudioParamEnabledDependenciesExcludingSelf(control);
    if (filteredDependencies.size() > 0)
    {
      result += "\r\nDisabled By: ";
      for (int i = 0; i < (int)filteredDependencies.size(); i++)
      {
        result += HostContext()->Topo()->audio.params[filteredDependencies[i]].displayName;
        if (i < (int)filteredDependencies.size() - 1)
          result += ", ";
      }
    }
  }
  result += "\r\nStored In: " + (param.static_.storeInPatch ? std::string("Session And Patch") : std::string("Session Only"));

  double modMin = 1.0;
  double modMax = 0.0;
  FBParamModulationBoundsSource source = GetParamModulationBounds(index, modMin, modMax);
  if (source == FBParamModulationBoundsSource::None)
    return result;

  result += "\r\n";
  if ((source & FBParamModulationBoundsSource::Matrix) != 0)
    result += "\r\nModulated By Matrix";
  if ((source & FBParamModulationBoundsSource::Unison) != 0)
    result += "\r\nModulated By Unison";
  if ((source & FBParamModulationBoundsSource::DirectAccess) != 0)
    result += "\r\nModulated By Direct Mod";

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

  // just dont want it
  if (dynamic_cast<TextEditor*>(event.eventComponent))
    return;
  if (dynamic_cast<Button*>(event.eventComponent))
    return;

  // pops up host context menu
  if (dynamic_cast<FBParamControl*>(event.eventComponent))
    return;

  // pops up gui param context menu
  if (dynamic_cast<FBGUIParamControl*>(event.eventComponent))
    return;

  // for combos
  if (event.eventComponent && event.eventComponent->findParentComponentOfClass<FBParamControl>())
    return;
  if (event.eventComponent && event.eventComponent->findParentComponentOfClass<FBGUIParamControl>())
    return;

  // pops up module context menu
  if (dynamic_cast<FBTabBarButton*>(event.eventComponent))
    return;

  auto& undoState = HostContext()->UndoState();
  PopupMenu menu;
  menu.addItem(1, "About");
  menu.addItem(2, "Show Manual");
  menu.addItem(3, "Clear Param Names");
  menu.addSeparator();
  menu.addItem(4, "Copy Patch");
  menu.addItem(5, "Paste Patch");
  menu.addSeparator();
  menu.addItem(6, "Dump Topology");
  menu.addItem(7, "Dump Param List");
  menu.addItem(8, "Show Log Folder");
  menu.addItem(9, "Show Plugin Folder");
  menu.addSeparator();
  if (undoState.CanUndo() || undoState.CanRedo())
    menu.addSeparator();
  if (undoState.CanUndo())
    menu.addItem(10, "Undo " + undoState.UndoAction());
  if (undoState.CanRedo())
    menu.addItem(11, "Redo " + undoState.RedoAction());

  PopupMenu::Options options;
  auto lnf = FBGetLookAndFeelFor(this);
  options = options.withMousePosition();
  options = options.withParentComponent(this);
  options = options.withStandardItemHeight(lnf->GetStandardPopupMenuItemHeight());
  menu.showMenuAsync(options, [this](int id) {
    if (id == 1) ShowAboutBox();
    if (id == 2) HostContext()->ShowOnlineManual();
    if (id == 6) DumpTopologyToFile();
    if (id == 7) DumpParamListToFile();
    if (id == 8) ShowLogFolder();
    if (id == 9) ShowPluginFolder();
    if (id == 10) HostContext()->UndoState().Undo();
    if (id == 11) HostContext()->UndoState().Redo();
    if (id == 3) {
      HostContext()->ClearAudioParamNameOverrides();
      HostContext()->NotifyHostOfParamNameChanges();
    }
    if (id == 4) {
      FBScalarStateContainer editState(*HostContext()->Topo());
      editState.CopyFrom(HostContext(), true);
      SystemClipboard::copyTextToClipboard(HostContext()->Topo()->SavePatchStateToString(editState, *HostContext()));
    }
    if (id == 5) {
      if(!LoadPatchFromText("Paste Patch", "Paste Patch", SystemClipboard::getTextFromClipboard().toStdString()))
        AlertWindow::showMessageBoxAsync(
          MessageBoxIconType::NoIcon,
          "Warning",
          "No valid patch data found on clipboard.");
    }  
  });
}

void
FBPlugGUI::ReloadPatch()
{
  FB_LOG_ENTRY_EXIT();
  BeforePatchChanged();
  std::string oldName = HostContext()->PatchName();
  HostContext()->UndoState().Snapshot("Reload Patch");
  HostContext()->RevertPatchToPatchState();
  HostContext()->MarkPatchAsPatchState(oldName);
  HostContext()->NotifyHostOfParamNameChanges();
  AfterPatchChanged(false);
}

void
FBPlugGUI::ReloadSession()
{
  FB_LOG_ENTRY_EXIT();
  BeforePatchChanged();
  HostContext()->UndoState().Snapshot("Reload Session");
  HostContext()->RevertPatchToSessionState();
  HostContext()->NotifyHostOfParamNameChanges();
  AfterPatchChanged(false);
}

void
FBPlugGUI::LoadPatchFromFile(bool preset)
{
  FB_LOG_ENTRY_EXIT();
  HideAllOverlaysAndFileBrowsers();
  InitLoadPatchBrowser(preset);
  _patchWasPreviewed = false;
  _patchBeforePreview.CopyFrom(HostContext(), true);
  _loadPatchBrowser->Show();
}

void
FBPlugGUI::SavePatchToFile()
{
  FB_LOG_ENTRY_EXIT();
  HideAllOverlaysAndFileBrowsers();
  InitSavePatchBrowser();
  _savePatchBrowser->Show();
}

void
FBPlugGUI::DumpTopologyToFile()
{
  FB_LOG_ENTRY_EXIT();
  HideAllOverlaysAndFileBrowsers();
  InitDumpTopologyBrowser();
  _saveTopologyBrowser->Show();
}

void
FBPlugGUI::DumpParamListToFile()
{
  FB_LOG_ENTRY_EXIT();
  HideAllOverlaysAndFileBrowsers();
  InitDumpParamListBrowser();
  _saveParamListBrowser->Show();
}

void
FBPlugGUI::ShowLogFolder()
{
  auto path = FBGetLogPath(HostContext()->Topo()->static_->meta);
  File(path.string()).revealToUser();
}

void
FBPlugGUI::ShowPluginFolder()
{
  auto path = FBGetPluginContentsFolderPath();
  File(path.string()).revealToUser();
}

void
FBPlugGUI::ShowAboutBox()
{
  auto const& meta = HostContext()->Topo()->static_->meta;
  ShowOverlayComponent(meta.name, 0, 0, _aboutBoxStack, 300, 120, false, []() {}, []() {});
}

void
FBPlugGUI::HideAllOverlaysAndFileBrowsers()
{
  HideOverlayComponent();
  _loadPatchBrowser->Hide();
  _savePatchBrowser->Hide();
  _saveTopologyBrowser->Hide();
  _saveParamListBrowser->Hide();
}

void 
FBPlugGUI::InitPatch()
{
  FB_LOG_ENTRY_EXIT();
  BeforePatchChanged();
  HostContext()->UndoState().Snapshot("Init Patch");
  FBScalarStateContainer defaultState(*HostContext()->Topo());
  for (int i = 0; i < defaultState.Params().size(); i++)
    if(HostContext()->Topo()->audio.params[i].static_.storeInPatch)
      HostContext()->PerformImmediateAudioParamEdit(i, *defaultState.Params()[i]);
  HostContext()->ClearAudioParamNameOverrides();
  HostContext()->MarkPatchAsPatchState("Init Patch");
  HostContext()->NotifyHostOfParamNameChanges();
  AfterPatchChanged(false);
}

void 
FBPlugGUI::RevertPreviewedPatch()
{
  if (!_patchWasPreviewed)
    return;
  BeforePatchChanged();
  _patchBeforePreview.CopyTo(HostContext(), true);
  AfterPatchChanged(false);
}

void 
FBPlugGUI::LoadPatchAsPreview(
  juce::File const& file)
{
  FB_LOG_ENTRY_EXIT();
  _patchWasPreviewed = true;
  std::map<int, std::string> previewParamNames;
  FBScalarStateContainer editState(*HostContext()->Topo());
  if (!HostContext()->Topo()->LoadPatchStateFromString(file.loadFileAsString().toStdString(), editState, previewParamNames))
    return;
  BeforePatchChanged();
  editState.CopyTo(HostContext(), true);
  AfterPatchChanged(true);
}

bool
FBPlugGUI::LoadPatchFromText(
  std::string const& undoAction,
  std::string const& patchName,
  std::string const& text)
{
  FB_LOG_ENTRY_EXIT();
  RevertPreviewedPatch();
  FBScalarStateContainer editState(*HostContext()->Topo());
  HostContext()->UndoState().Snapshot(undoAction);
  if (!HostContext()->Topo()->LoadPatchStateFromString(text, editState, *HostContext()))
    return false;
  BeforePatchChanged();
  editState.CopyTo(HostContext(), true);
  HostContext()->MarkPatchAsPatchState(patchName);
  HostContext()->NotifyHostOfParamNameChanges();
  AfterPatchChanged(false);
  return true;
}

void
FBPlugGUI::SetupAboutBoxGUI()
{
#if FB_AARCH64      
  std::string archName = "ARM";
#else
  std::string archName = "X64";
#endif
  auto const& meta = HostContext()->Topo()->static_->meta;
  _aboutBoxStack = StoreComponent<FBStackingComponent>();
  auto image = StoreComponent<FBImageComponent>(this, FBThemeResourceId::AboutBoxImageFileName, 200, RectanglePlacement::Flags::centred);
  auto grid = StoreComponent<FBGridComponent>(this, true, -1, -1, std::vector<int> { { 0, 0, 0, 0 } }, std::vector<int> { { 1 } });
  grid->Add(0, 0, StoreComponent<FBAutoSizeLabel>(this, "Arch: " + archName));
  grid->Add(1, 0, StoreComponent<FBAutoSizeLabel>(this, "Version: " + meta.version.ToString()));
  grid->Add(2, 0, StoreComponent<FBAutoSizeLabel>(this, "Format: " + FBPlugFormatToString(meta.format)));
  grid->Add(3, 0, StoreComponent<FBAutoSizeLabel>(this, "Plugin ID: " + meta.id));
  _aboutBoxStack->addAndMakeVisible(image, 0);
  _aboutBoxStack->addAndMakeVisible(grid, 1);
}

void
FBPlugGUI::SetupOverlayGUI()
{
  _overlayGrid = StoreComponent<FBGridComponent>(this, true, -1, -1, std::vector<int> { { 0, 1 } }, std::vector<int> { { 1, 1 } });
  _overlayCaption = StoreComponent<FBAutoSizeLabel2>(false, true, true, 200);
  _overlayGrid->Add(0, 0, _overlayCaption);

  _overlayInitClose = StoreComponent<FBGridComponent>(this, true, -1, -1, std::vector<int> { { 0 } }, std::vector<int> { { 0, 0, 1 } });
  auto overlayClose = StoreComponent<FBAutoSizeButton>(this, "Close");
  overlayClose->onClick = [this] { _overlayClose(); HideOverlayComponent(); };
  auto overlayCloseSection = StoreComponent<FBMarginComponent>(this, false, true, true, true, overlayClose);
  _overlayInitClose->Add(0, 0, overlayCloseSection);
  _overlayInitButton = StoreComponent<FBAutoSizeButton>(this, "Init");
  _overlayInitButton->onClick = [this] { _overlayInit(); };
  auto overlayInitSection = StoreComponent<FBMarginComponent>(this, false, false, true, true, _overlayInitButton);
  _overlayInitClose->Add(0, 1, overlayInitSection);
  _overlayGrid->Add(0, 1, _overlayInitClose);

  _overlayContent = StoreComponent<FBContentComponent>();
  _overlayGrid->Add(1, 0, 1, 2, _overlayContent);
  _overlayGrid->MarkSection({ { 0, 0 }, { 1, 2 } }, FBGridSectionMark::DefaultBackground);

  _overlayCard = StoreComponent<FBCardComponent>(this, _overlayGrid);
  _overlayModule = StoreComponent<FBModuleComponent>(HostContext()->Topo());
  _overlayOuterMargin = StoreComponent<FBMarginComponent>(this, true, true, true, true, _overlayModule, true);
}

void
FBPlugGUI::HideOverlayComponent()
{
  if (_overlayComponent == nullptr)
    return;
  _overlayInit = [](){};
  _overlayClose = [](){};
  _overlayComponent->setVisible(false);
  _overlayContent->SetContent(nullptr);
  _overlayOuterMargin->setVisible(false);
  _overlayCaption->setText("", dontSendNotification);
  removeChildComponent(_overlayOuterMargin);
} 

void
FBPlugGUI::ShowOverlayComponent(
  std::string const& title,
  int moduleIndex, int moduleSlot,
  Component* overlay,
  int w, int h, bool hasInit,
  std::function<void()> init,
  std::function<void()> close)
{
  HideAllOverlaysAndFileBrowsers();
  int x = (getWidth() - w) / 2;
  int y = (getHeight() - h) / 2;
  _overlayInit = init;
  _overlayClose = close;
  _overlayInitButton->setVisible(hasInit);
  _overlayContent->SetContent(overlay);
  _overlayOuterMargin->setBounds(x, y, w, h);
  _overlayCaption->setText(title, dontSendNotification);
  addAndMakeVisible(_overlayOuterMargin, 1);
  _overlayModule->SetModuleContent(moduleIndex, moduleSlot, _overlayCard);
  _overlayOuterMargin->resized();
  _overlayComponent = overlay;
}