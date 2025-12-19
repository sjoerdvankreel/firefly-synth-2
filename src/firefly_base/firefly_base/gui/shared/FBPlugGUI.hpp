#pragma once

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class FBParamControl;
class FBGUIParamControl;
class FBParamsDependent;
class FBHostGUIContext;
class FBContentComponent;
struct FBParamTopoIndices;

// Well here goes my nice distinction between plug and base library again.
enum FBParamModulationBoundsSource
{
  None = 0x0,
  Matrix = 0x1,
  Unison = 0x2,
  DirectAccess = 0x4
};

class IFBParamListener
{
public:
  virtual ~IFBParamListener() {}
  virtual void AudioParamChanged(int index, double normalized, bool changedFromUI) = 0;
};

class FBPlugGUI:
public juce::Component,
public IFBHostGUIContextListener
{
  double _scale = 1.0;
  juce::Label* _overlayCaption = {};
  std::function<void()> _overlayInit = {};
  FBGridComponent* _overlayGrid = {};
  juce::Component* _overlayComponent = {};
  FBContentComponent* _overlayContent = {};
  FBModuleComponent* _overlayContainer = {};
  std::vector<IFBParamListener*> _paramListeners = {};

  bool LoadPatchFromText(
    std::string const& undoAction, 
    std::string const& patchName,
    std::string const& text);

  juce::PopupMenu MakePresetMenu(
    std::shared_ptr<FBPresetFolder> folder);

public:
  virtual ~FBPlugGUI();

  template <class TComponent, class... Args>
  TComponent* StoreComponent(Args&&... args);

  void ShowPopupMenuFor(
    juce::Component* target,
    juce::PopupMenu menu,
    std::function<void(int)> callback);

  void InitPatch();
  void ReloadPatch();
  void ReloadSession();
  void SavePatchToFile();
  void LoadPatchFromFile();
  void LoadPreset(juce::Component* clickedFrom);
  
  FBTheme const& GetTheme() const;
  void SwitchTheme(std::string const& themeName);
  std::vector<FBTheme> const& Themes() const { return _themes; }

  void HideOverlayComponent();
  void ShowOverlayComponent(
    std::string const& title,
    int moduleIndex, int moduleSlot,
    juce::Component* overlay,
    int w, int h, bool vCenter,
    std::function<void()> init);

  void AddParamListener(IFBParamListener* listener);
  void RemoveParamListener(IFBParamListener* listener);

  std::string GetTooltipForGUIParam(int index) const;
  std::string GetTooltipForAudioParam(int index) const;
  FBHostGUIContext* HostContext() const { return _hostContext; }

  void OnPatchLoaded() override {}
  void OnPatchNameChanged(std::string const& /*name*/) override {}
  void mouseUp(const juce::MouseEvent& event) override;

  void HideTooltip();
  void SetTooltip(juce::Point<int> const& screenPos, juce::String const& text);

  void UpdateExchangeState();
  void SetScale(double scale);
  void ShowMenuForGUIParam(int index);
  void ShowMenuForAudioParam(int index, bool showHostMenu);
  int GetControlCountForAudioParamIndex(int paramIndex) const;
  FBParamControl* GetControlForAudioParamIndex(int paramIndex, int controlIndex) const;
  void RepaintSlidersForAudioParam(FBParamTopoIndices const& indices);

  virtual void ModuleSlotClicked(int index, int slot) = 0;
  virtual void ActiveModuleSlotChanged(int index, int slot) = 0;
  virtual void GUIParamNormalizedChanged(int index, double normalized);
  virtual void AudioParamNormalizedChangedFromUI(int index, double normalized);
  virtual void AudioParamNormalizedChangedFromHost(int index, double normalized);

  virtual bool HighlightModulationBounds() const = 0;
  virtual FBHighlightTweakMode HighlightTweakedMode() const = 0;
  virtual FBGUIRenderType GetRenderType(bool graphOrKnob) const = 0;
  virtual FBParamModulationBoundsSource GetParamModulationBounds(int index, double& minNorm, double& maxNorm) const = 0;

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUI);
  FBPlugGUI(FBHostGUIContext* hostContext);

  virtual void OnPatchChanged() = 0;
  virtual void UpdateExchangeStateTick() = 0;

  void InitAllDependencies();
  FBGUIParamControl* GetControlForGUIParamIndex(int paramIndex) const;
  juce::Component* StoreComponent(std::unique_ptr<juce::Component>&& component);

private:
  void SetupOverlayGUI();
  void GUIParamNormalizedChanged(int index);
  void AudioParamNormalizedChanged(int index);

  std::vector<FBTheme> _themes = {};
  FBHostGUIContext* const _hostContext;
  juce::TooltipWindow* _tooltipWindow = {};
  std::vector<std::unique_ptr<juce::Component>> _store = {};
  std::unordered_map<int, int> _guiParamIndexToComponent = {};
  std::unordered_map<int, std::vector<int>> _audioParamIndexToComponents = {};
  std::chrono::high_resolution_clock::time_point _exchangeUpdated = {};
  std::unordered_map<int, std::unordered_set<FBParamsDependent*>> _guiParamsVisibleDependents = {};
  std::unordered_map<int, std::unordered_set<FBParamsDependent*>> _guiParamsEnabledDependents = {};
  std::unordered_map<int, std::unordered_set<FBParamsDependent*>> _audioParamsVisibleDependents = {};
  std::unordered_map<int, std::unordered_set<FBParamsDependent*>> _audioParamsEnabledDependents = {};
};

template <class TComponent, class... Args>
TComponent* FBPlugGUI::StoreComponent(Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  StoreComponent(std::move(component));
  return result;
}