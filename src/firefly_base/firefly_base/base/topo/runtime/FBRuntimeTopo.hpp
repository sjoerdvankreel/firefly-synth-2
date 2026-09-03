#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeModule.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParamsTopo.hpp>

#include <juce_core/juce_core.h>
#include <map>
#include <string>
#include <vector>
#include <memory>

struct FBStaticTopo;
class FBHostGUIContext;
class FBGUIStateContainer;
class FBProcStateContainer;
class FBScalarStateContainer;

struct FBRuntimeTopo final
{
private:
  template <class TContainer, class TParam>
  juce::var SaveParamStateToVar(TContainer const& container, std::vector<TParam> const& params, bool patchOnly) const;
  template <class TContainer, class TParamsTopo>
  bool LoadParamStateFromVar(bool isGuiState, juce::var const& json, TContainer& container, TParamsTopo& params, bool patchOnly) const;

  juce::var SaveGUIStateToVar(FBHostGUIContext const& hostContext) const;
  juce::var SaveParamNameOverridesToVar(FBHostGUIContext const& hostContext) const;
  juce::var SaveProcStateToVar(FBProcStateContainer const& procState, bool patchOnly) const;
  juce::var SaveEditStateToVar(FBScalarStateContainer const& editState, bool patchOnly) const;
  juce::var SavePatchStateToVar(FBScalarStateContainer const& editState, FBHostGUIContext const& hostContext) const;
  juce::var SaveEditAndGUIStateToVar(FBScalarStateContainer const& editState, FBHostGUIContext const& hostContext, bool patchOnly) const;

  bool LoadGUIStateFromVar(juce::var const& json, FBHostGUIContext& hostContext) const;
  bool LoadProcStateFromVar(juce::var const& json, FBProcStateContainer& proc, bool patchOnly) const;
  bool LoadEditStateFromVar(juce::var const& json, FBScalarStateContainer& edit, bool patchOnly) const;
  bool LoadParamNameOverridesFromVar(juce::var const& json, std::map<int, std::string>& paramNameOverrides) const;
  bool LoadPatchStateFromVar(juce::var const& json, FBScalarStateContainer& edit, FBHostGUIContext& hostContext) const;
  bool LoadPatchStateFromVar(juce::var const& json, FBScalarStateContainer& editState, std::map<int, std::string>& paramNameOverrides) const;
  bool LoadGUIStateFromVar(juce::var const& json, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides) const;
  bool LoadEditAndGUIStateFromVar(juce::var const& json, FBScalarStateContainer& edit, FBHostGUIContext& hostContext, bool patchOnly) const;
  bool LoadEditAndGUIStateFromVar(juce::var const& json, FBScalarStateContainer& edit, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, bool patchOnly) const;

public:
  std::unique_ptr<FBStaticTopo> static_;
  std::vector<FBRuntimeModule> modules;
  FBRuntimeParamsTopo<FBRuntimeParam> audio;
  FBRuntimeParamsTopo<FBRuntimeGUIParam> gui;
  std::map<FBTopoIndices, int> moduleTopoToRuntime;

  FB_NOCOPY_MOVE_NODEFCTOR(FBRuntimeTopo);
  FBRuntimeTopo(std::unique_ptr<FBStaticTopo>&& topo);

  std::string PrintParamList() const;
  FBRuntimeModule const* ModuleAtTopo(FBTopoIndices const& topoIndices) const;

  std::string SaveGUIStateToString(FBHostGUIContext const& hostContext) const;
  std::string SaveProcStateToString(FBProcStateContainer const& procState, bool patchOnly) const;
  std::string SaveEditStateToString(FBScalarStateContainer const& editState, bool patchOnly) const;
  std::string SavePatchStateToString(FBScalarStateContainer const& editState, FBHostGUIContext const& hostContext) const;
  std::string SaveEditAndGUIStateToString(FBScalarStateContainer const& editState, FBHostGUIContext const& hostContext, bool patchOnly) const;

  bool LoadGUIStateFromString(std::string const& text, FBHostGUIContext& hostContext) const;
  bool LoadProcStateFromString(std::string const& text, FBProcStateContainer& procState, bool patchOnly) const;
  bool LoadEditStateFromString(std::string const& text, FBScalarStateContainer& editState, bool patchOnly) const;
  bool LoadPatchStateFromString(std::string const& text, FBScalarStateContainer& editState, FBHostGUIContext& hostContext) const;
  bool LoadPatchStateFromString(std::string const& text, FBScalarStateContainer& editState, std::map<int, std::string>& paramNameOverrides) const;
  bool LoadGUIStateFromString(std::string const& text, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides) const;
  bool LoadEditAndGUIStateFromString(std::string const& text, FBScalarStateContainer& editState, FBHostGUIContext& hostContext, bool patchOnly) const;
  bool LoadEditAndGUIStateFromString(std::string const& text, FBScalarStateContainer& editState, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, bool patchOnly) const;

  // With dry run copies over the loaded state only if load succeeded.
  // It returns void rather than bool because it "always succeeds" even if not.
  // This is important to keep bitwig happy (i.e. do nothing on load fail) rather than crashing the host.
  void LoadGUIStateFromStringWithDryRun(std::string const& text, FBHostGUIContext& hostContext) const;
  void LoadProcStateFromStringWithDryRun(std::string const& text, FBProcStateContainer& procState, bool patchOnly) const;
  void LoadEditAndGUIStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& editState, FBHostGUIContext& hostContext, bool patchOnly) const;
};