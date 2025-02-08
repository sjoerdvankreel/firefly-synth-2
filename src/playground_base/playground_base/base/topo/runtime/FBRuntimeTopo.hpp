#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeModule.hpp>
#include <playground_base/base/topo/runtime/FBParamTopoIndices.hpp>

#include <juce_core/juce_core.h>

#include <map>
#include <vector>
#include <unordered_map>

struct FBGUIState;
class FBProcStateContainer;
class FBScalarStateContainer;

struct FBRuntimeTopo final
{
  FBStaticTopo static_;
  std::vector<FBRuntimeModule> modules;
  std::vector<FBRuntimeParam> params;
  std::unordered_map<int, int> paramTagToIndex;
  std::map<FBParamTopoIndices, int> paramTopoToRuntime;

  FBRuntimeTopo(FBStaticTopo const& static_);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);

  FBRuntimeModule const* ModuleAtParamIndex(int runtimeParamIndex) const;
  FBRuntimeParam const* ParamAtTopo(FBParamTopoIndices const& topoIndices) const;

  juce::var SaveGUIStateToVar(FBGUIState const& gui) const;
  juce::var SaveProcStateToVar(FBProcStateContainer const& proc) const;
  juce::var SaveEditStateToVar(FBScalarStateContainer const& edit) const;
  juce::var SaveEditAndGUIStateToVar(FBScalarStateContainer const& edit, FBGUIState const& gui) const;

  std::string SaveGUIStateToString(FBGUIState const& gui) const;
  std::string SaveProcStateToString(FBProcStateContainer const& proc) const;
  std::string SaveEditStateToString(FBScalarStateContainer const& edit) const;
  std::string SaveEditAndGUIStateToString(FBScalarStateContainer const& edit, FBGUIState const& gui) const;

  bool LoadGUIStateFromVar(juce::var const& json, FBGUIState& gui) const;
  bool LoadProcStateFromVar(juce::var const& json, FBProcStateContainer& proc) const;
  bool LoadEditStateFromVar(juce::var const& json, FBScalarStateContainer& edit) const;
  bool LoadEditAndGUIStateFromVar(juce::var const& json, FBScalarStateContainer& edit, FBGUIState& gui) const;

  bool LoadGUIStateFromString(std::string const& text, FBGUIState& gui) const;
  bool LoadProcStateFromString(std::string const& text, FBProcStateContainer& proc) const;
  bool LoadEditStateFromString(std::string const& text, FBScalarStateContainer& edit) const;
  bool LoadEditAndGUIStateFromString(std::string const& text, FBScalarStateContainer& edit, FBGUIState& gui) const;

  bool LoadGUIStateFromStringWithDryRun(std::string const& text, FBGUIState& gui) const;
  bool LoadProcStateFromStringWithDryRun(std::string const& text, FBProcStateContainer& proc) const;
  bool LoadEditStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& edit) const;
  bool LoadEditAndGUIStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& edit, FBGUIState& gui) const;
};