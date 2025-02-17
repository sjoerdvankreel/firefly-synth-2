#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <chrono>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class FBHostGUIContext;

class FBPlugGUI:
public juce::Component
{
public:
  template <class TComponent, class... Args>
  TComponent* StoreComponent(Args&&... args);

  void ShowPopupMenuFor(
    juce::Component* target,
    juce::PopupMenu menu,
    std::function<void(int)> callback);

  std::string GetTooltipForAudioParam(int index) const;
  std::string GetAudioParamActiveTooltip(
    FBStaticParam const& param, bool active, float value) const;

  void UpdateExchangeState();
  void ShowHostMenuForAudioParam(int index);
  void SteppedAudioParamNormalizedChanged(int index);
  FBHostGUIContext* HostContext() const { return _hostContext; }

  virtual void ActiveModuleSlotChanged(int index, int slot) = 0;
  virtual void SetAudioParamNormalizedFromUI(int index, float normalized) = 0;
  virtual void SetAudioParamNormalizedFromHost(int index, float normalized);

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUI);
  FBPlugGUI(FBHostGUIContext* hostContext);

  void InitAllDependencies();
  virtual void UpdateExchangeStateTick() = 0;
  FBParamControl* GetControlForAudioParamIndex(int paramIndex) const;
  juce::Component* StoreComponent(std::unique_ptr<juce::Component>&& component);

private:
  FBHostGUIContext* const _hostContext;
  juce::TooltipWindow* _tooltipWindow = {};
  std::vector<std::unique_ptr<juce::Component>> _store = {};
  std::unordered_map<int, int> _guiParamIndexToComponent = {};
  std::unordered_map<int, int> _audioParamIndexToComponent = {};
  std::chrono::high_resolution_clock::time_point _exchangeUpdated = {};
  std::unordered_map<int, std::unordered_set<FBParamsDependent*>> _paramsVisibleDependents = {};
  std::unordered_map<int, std::unordered_set<FBParamsDependent*>> _paramsEnabledDependents = {};
};

template <class TComponent, class... Args>
TComponent* FBPlugGUI::StoreComponent(Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  StoreComponent(std::move(component));
  return result;
}