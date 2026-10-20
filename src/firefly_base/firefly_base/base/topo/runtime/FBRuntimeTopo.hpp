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
class FBHostDSPContext;
class FBHostGUIContext;
class FBGUIStateContainer;
class FBProcStateContainer;
class FBScalarStateContainer;

// Note to self:
// VST3 processor/controller separation really gets in the way.
// Implementing the sampler made that 100% clear.
// I'm not too thrilled about going with SingleComponentEffect this far in the project 
// but i dropped kDistributable at least to guarantee single-process (see PullDSPAudioClipsToGUI).
// So, current solution:
// 1: on sample load button we make a trimmed down copy (1000 samples or so) of the loaded sample
// 2: store that on the gui side, send the original to dsp side
// 3: on daw session load all is good, we can put everything right back where it belongs
// 4: on patch save, we need to get samples back from dsp to gui, to store it in the patch
// 5: on patch load, we need to get samples back from gui to dsp, just like sample load button
// 6: for clap gui->dsp is doable: gui->dsp pushes through a single queue, dsp eventually picks up
// 7: for vst3 gui->dsp is "doable": we first go through IMessage and then same sort of queue as for clap
// 8: for both, dsp->gui is meh: need to wait for the gui->dsp queue to clear, take locks everywhere, copy from in-process.
struct FBRuntimeTopo final
{
private:
  template <class TContainer, class TParam>
  juce::var SaveParamStateToVar(TContainer const& container, std::vector<TParam> const& params, bool patchOnly) const;
  template <class TContainer, class TParamsTopo>
  bool LoadParamStateFromVar(bool isGuiState, juce::var const& json, TContainer& container, TParamsTopo& params, bool patchOnly) const;

  juce::var SaveGUIStateToVar(FBHostGUIContext const& hostContext) const;
  juce::var SaveParamNameOverridesToVar(FBHostGUIContext const& hostContext) const;
  juce::var SaveAudioClipsToVar(std::map<std::string, FBAudioClip> const& audioClips) const;
  juce::var SaveVST3ProcStateToVar(FBProcStateContainer const& procState, bool patchOnly) const;
  juce::var SaveEditStateToVar(FBScalarStateContainer const& editState, bool patchOnly) const;
  juce::var SavePatchStateToVar(FBScalarStateContainer const& editState, FBHostGUIContext const& hostContext) const;
  juce::var SaveCLAPEverythingToVar(FBScalarStateContainer const& editState, FBHostGUIContext const& hostGUIContext, FBHostDSPContext const& hostDSPContext, bool patchOnly) const;

  bool LoadGUIStateFromVar(juce::var const& json, FBHostGUIContext& hostContext) const;
  bool LoadEditStateFromVar(juce::var const& json, FBScalarStateContainer& edit, bool patchOnly) const;
  bool LoadVST3EditStateFromVar(juce::var const& json, FBScalarStateContainer& edit, bool patchOnly) const; // hint: mess
  bool LoadAudioClipsFromVar(juce::var const& json, std::map<std::string, FBAudioClip>& audioClips) const;
  bool LoadParamNameOverridesFromVar(juce::var const& json, std::map<int, std::string>& paramNameOverrides) const;
  void LoadVST3ProcStateFromVarWithDryRun(juce::var const& json, FBProcStateContainer& procState, bool patchOnly) const;
  bool LoadPatchStateFromVar(juce::var const& json, FBScalarStateContainer& edit, FBHostGUIContext& hostContext) const;
  bool LoadPatchStateFromVar(juce::var const& json, FBScalarStateContainer& editState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips, std::map<std::string, FBAudioClip>& dspAudioClips) const;
  bool LoadGUIStateFromVar(juce::var const& json, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips) const;
  bool LoadCLAPEverythingFromVar(juce::var const& json, FBScalarStateContainer& edit, FBHostGUIContext& hostGUIContext, FBHostDSPContext& hostDSPContext, bool patchOnly) const;
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

  std::string SaveGUIStateToString(FBHostGUIContext const& hostContext) const;
  std::string SaveEditStateToString(FBScalarStateContainer const& editState, bool patchOnly) const;
  std::string SaveVST3ProcStateToString(FBProcStateContainer const& procState, bool patchOnly) const;
  std::string SavePatchStateToString(FBScalarStateContainer const& editState, FBHostGUIContext const& hostContext) const;
  std::string SaveCLAPEverythingToString(FBScalarStateContainer const& editState, FBHostGUIContext const& hostGUIContext, FBHostDSPContext const& hostDSPContext, bool patchOnly) const;

  bool LoadGUIStateFromString(std::string const& text, FBHostGUIContext& hostContext) const;
  bool LoadEditStateFromString(std::string const& text, FBScalarStateContainer& editState, bool patchOnly) const;
  bool LoadVST3EditStateFromString(std::string const& text, FBScalarStateContainer& editState, bool patchOnly) const; // hint: mess
  bool LoadPatchStateFromString(std::string const& text, FBScalarStateContainer& editState, FBHostGUIContext& hostContext) const;
  bool LoadPatchStateFromString(std::string const& text, FBScalarStateContainer& editState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips, std::map<std::string, FBAudioClip>& dspAudioClips) const;
  bool LoadGUIStateFromString(std::string const& text, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips) const;
  bool LoadCLAPEverythingFromString(std::string const& text, FBScalarStateContainer& editState, FBHostGUIContext& hostGUIContext, FBHostDSPContext& hostDSPContext, bool patchOnly) const;
  bool LoadCLAPEverythingFromString(std::string const& text, FBScalarStateContainer& editState, FBGUIStateContainer& guiState, std::map<int, std::string>& paramNameOverrides, std::map<std::string, FBAudioClip>& guiAudioClips, std::map<std::string, FBAudioClip>& dspAudioClips, bool patchOnly) const;

  // With dry run copies over the loaded state only if load succeeded.
  // It returns void rather than bool because it "always succeeds" even if not.
  // This is important to keep bitwig happy (i.e. do nothing on load fail) rather than crashing the host.
  void LoadVST3GUIStateFromStringWithDryRun(std::string const& text, FBHostGUIContext& hostContext) const;
  void LoadVST3ProcStateFromStringWithDryRun(std::string const& text, FBProcStateContainer& procState, bool patchOnly) const;
  void LoadCLAPEverythingFromStringWithDryRun(std::string const& text, FBScalarStateContainer& editState, FBHostGUIContext& hostGUIContext, FBHostDSPContext& hostDSPContext, bool patchOnly) const;
};