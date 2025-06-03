#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeModule.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParamsTopo.hpp>

#include <juce_core/juce_core.h>

#include <map>
#include <vector>
#include <unordered_map>

class FBGUIStateContainer;
class FBProcStateContainer;
class FBScalarStateContainer;

struct FBRuntimeTopo final
{
private:
  template <class TContainer, class TParam> 
  juce::var SaveParamStateToVar(TContainer const& container, std::vector<TParam> const& params) const;
  template <class TContainer, class TParamsTopo>
  bool LoadParamStateFromVar(juce::var const& json, TContainer& container, TParamsTopo& params) const;

public:
  FBStaticTopo static_;
  std::vector<FBRuntimeModule> modules;
  FBRuntimeParamsTopo<FBRuntimeParam> audio;
  FBRuntimeParamsTopo<FBRuntimeGUIParam> gui;
  std::map<FBTopoIndices, int> moduleTopoToRuntime;

  FBRuntimeTopo(FBStaticTopo const& static_);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);
  FBRuntimeModule const* ModuleAtTopo(FBTopoIndices const& topoIndices) const;

  juce::var SaveGUIStateToVar(FBGUIStateContainer const& gui) const;
  juce::var SaveProcStateToVar(FBProcStateContainer const& proc) const;
  juce::var SaveEditStateToVar(FBScalarStateContainer const& edit) const;
  juce::var SaveEditAndGUIStateToVar(FBScalarStateContainer const& edit, FBGUIStateContainer const& gui) const;

  std::string SaveGUIStateToString(FBGUIStateContainer const& gui) const;
  std::string SaveProcStateToString(FBProcStateContainer const& proc) const;
  std::string SaveEditStateToString(FBScalarStateContainer const& edit) const;
  std::string SaveEditAndGUIStateToString(FBScalarStateContainer const& edit, FBGUIStateContainer const& gui) const;

  bool LoadGUIStateFromVar(juce::var const& json, FBGUIStateContainer& gui) const;
  bool LoadProcStateFromVar(juce::var const& json, FBProcStateContainer& proc) const;
  bool LoadEditStateFromVar(juce::var const& json, FBScalarStateContainer& edit) const;
  bool LoadEditAndGUIStateFromVar(juce::var const& json, FBScalarStateContainer& edit, FBGUIStateContainer& gui) const;

  bool LoadGUIStateFromString(std::string const& text, FBGUIStateContainer& gui) const;
  bool LoadProcStateFromString(std::string const& text, FBProcStateContainer& proc) const;
  bool LoadEditStateFromString(std::string const& text, FBScalarStateContainer& edit) const;
  bool LoadEditAndGUIStateFromString(std::string const& text, FBScalarStateContainer& edit, FBGUIStateContainer& gui) const;

  // With dry run copies over the loaded state only if load succeeded.
  // It returns void rather than bool because it "always succeeds" even if not.
  // This is important to keep bitwig happy (i.e. do nothing on load fail) rather than crashing the host.
  void LoadGUIStateFromStringWithDryRun(std::string const& text, FBGUIStateContainer& gui) const;
  void LoadProcStateFromStringWithDryRun(std::string const& text, FBProcStateContainer& proc) const;
  void LoadEditStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& edit) const;
  void LoadEditAndGUIStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& edit, FBGUIStateContainer& gui) const;
};