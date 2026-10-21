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
class FBHostContext;
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

  juce::var SaveGUIStateToVar(FBHostContext const& hostContext) const;
  juce::var SaveParamNameOverridesToVar(FBHostContext const& hostContext) const;
  juce::var SaveAudioClipsToVar(std::map<std::string, FBAudioClip> const& audioClips) const;
  juce::var SaveVST3ProcStateToVar(FBProcStateContainer const& procState, bool patchOnly) const;
  juce::var SaveEditStateToVar(FBScalarStateContainer const& editState, bool patchOnly) const;
  juce::var SavePatchStateToVar(FBScalarStateContainer const& editState, FBHostContext const& hostContext) const;
  juce::var SaveCLAPEverythingToVar(FBScalarStateContainer const& editState, FBHostContext const& hostContext, bool patchOnly) const;

  bool LoadGUIStateFromVar(juce::var const& json, FBHostContext& hostContext) const;
  bool LoadEditStateFromVar(juce::var const& json, FBScalarStateContainer& edit, bool patchOnly) const;
  bool LoadVST3EditStateFromVar(juce::var const& json, FBScalarStateContainer& edit, bool patchOnly) const; // hint: mess
  bool LoadAudioClipsFromVar(juce::var const& json, std::map<std::string, FBAudioClip>& audioClips) const;
  bool LoadParamNameOverridesFromVar(juce::var const& json, std::map<int, std::string>& paramNameOverrides) const;
  void LoadVST3ProcStateFromVarWithDryRun(juce::var const& json, FBProcStateContainer& procState, bool patchOnly) const;
  bool LoadPatchStateFromVar(juce::var const& json, FBScalarStateContainer& edit, FBHostContext& hostContext) const;
  bool LoadPatchStateFromVar(juce::var const& json, FBScalarStateContainer& editState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips, std::map<std::string, FBAudioClip>& dspAudioClips) const;
  bool LoadGUIStateFromVar(juce::var const& json, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips) const;
  bool LoadCLAPEverythingFromVar(juce::var const& json, FBScalarStateContainer& edit, FBHostContext& hostContext, bool patchOnly) const;
  bool LoadCLAPEverythingFromVar(juce::var const& json, FBScalarStateContainer& edit, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips, std::map<std::string, FBAudioClip>& dspAudioClips, bool patchOnly) const;

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

  std::string SaveGUIStateToString(FBHostContext const& hostContext) const;
  std::string SaveEditStateToString(FBScalarStateContainer const& editState, bool patchOnly) const;
  std::string SaveVST3ProcStateToString(FBProcStateContainer const& procState, bool patchOnly) const;
  std::string SavePatchStateToString(FBScalarStateContainer const& editState, FBHostContext const& hostContext) const;
  std::string SaveCLAPEverythingToString(FBScalarStateContainer const& editState, FBHostContext const& hostContext, bool patchOnly) const;

  bool LoadGUIStateFromString(std::string const& text, FBHostContext& hostContext) const;
  bool LoadEditStateFromString(std::string const& text, FBScalarStateContainer& editState, bool patchOnly) const;
  bool LoadVST3EditStateFromString(std::string const& text, FBScalarStateContainer& editState, bool patchOnly) const; // hint: mess
  bool LoadPatchStateFromString(std::string const& text, FBScalarStateContainer& editState, FBHostContext& hostContext) const;
  bool LoadPatchStateFromString(std::string const& text, FBScalarStateContainer& editState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips, std::map<std::string, FBAudioClip>& dspAudioClips) const;
  bool LoadGUIStateFromString(std::string const& text, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips) const;
  bool LoadCLAPEverythingFromString(std::string const& text, FBScalarStateContainer& editState, FBHostContext& hostContext, bool patchOnly) const;
  bool LoadCLAPEverythingFromString(std::string const& text, FBScalarStateContainer& editState, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips, std::map<std::string, FBAudioClip>& dspAudioClips, bool patchOnly) const;

  // With dry run copies over the loaded state only if load succeeded.
  // It returns void rather than bool because it "always succeeds" even if not.
  // This is important to keep bitwig happy (i.e. do nothing on load fail) rather than crashing the host.
  void LoadVST3GUIStateFromStringWithDryRun(std::string const& text, FBHostContext& hostContext) const;
  void LoadVST3ProcStateFromStringWithDryRun(std::string const& text, FBProcStateContainer& procState, bool patchOnly) const;
  void LoadCLAPEverythingFromStringWithDryRun(std::string const& text, FBScalarStateContainer& editState, FBHostContext& hostContext, bool patchOnly) const;
};