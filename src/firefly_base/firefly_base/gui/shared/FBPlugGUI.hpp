#pragma once

#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <chrono>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class FBHostGUIContext;

class IFBParamListener
{
public:
  virtual ~IFBParamListener() {}
  virtual void AudioParamChanged(int index, double normalized, bool changedFromUI) = 0;
};

class FBPlugGUI:
public juce::Component
{
  double _scale = 1.0;
  std::vector<IFBParamListener*> _paramListeners = {};

public:
  virtual ~FBPlugGUI();

  template <class TComponent, class... Args>
  TComponent* StoreComponent(Args&&... args);

  void ShowPopupMenuFor(
    juce::Component* target,
    juce::PopupMenu menu,
    std::function<void(int)> callback);

  void InitPatch();
  void SavePatchToFile();
  void LoadPatchFromFile();

  void AddParamListener(IFBParamListener* listener);
  void RemoveParamListener(IFBParamListener* listener);

  std::string GetTooltipForGUIParam(int index) const;
  std::string GetTooltipForAudioParam(int index) const;
  FBHostGUIContext* HostContext() const { return _hostContext; }

  void UpdateExchangeState();
  void SetScale(double scale);
  void ShowHostMenuForAudioParam(int index);

  void mouseUp(const juce::MouseEvent& event) override;

  virtual FBGUIRenderType GetKnobRenderType() const = 0;
  virtual FBGUIRenderType GetGraphRenderType() const = 0;
  virtual void ModuleSlotClicked(int index, int slot) = 0;
  virtual void ActiveModuleSlotChanged(int index, int slot) = 0;
  virtual void GUIParamNormalizedChanged(int index, double normalized);
  virtual void AudioParamNormalizedChangedFromUI(int index, double normalized);
  virtual void AudioParamNormalizedChangedFromHost(int index, double normalized);

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUI);
  FBPlugGUI(FBHostGUIContext* hostContext);

  virtual void OnPatchChanged() = 0;
  virtual void UpdateExchangeStateTick() = 0;

  void InitAllDependencies();
  int GetControlCountForAudioParamIndex(int paramIndex) const;
  FBGUIParamControl* GetControlForGUIParamIndex(int paramIndex) const;
  FBParamControl* GetControlForAudioParamIndex(int paramIndex, int controlIndex) const;
  juce::Component* StoreComponent(std::unique_ptr<juce::Component>&& component);

private:
  void GUIParamNormalizedChanged(int index);
  void AudioParamNormalizedChanged(int index);

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