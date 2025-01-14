#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeModule.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <juce_core/juce_core.h>
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

  FBRuntimeTopo(FBStaticTopo const& static_);
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeTopo);

  juce::var SaveGUIStateToVar(FBGUIState const& gui) const;
  juce::var SaveProcStateToVar(FBProcStateContainer const& proc) const;
  juce::var SaveEditStateToVar(FBScalarStateContainer const& edit) const;

  std::string SaveGUIStateToString(FBGUIState const& gui) const;
  std::string SaveProcStateToString(FBProcStateContainer const& proc) const;
  std::string SaveEditStateToString(FBScalarStateContainer const& edit) const;

  bool LoadGUIStateFromVar(juce::var const& json, FBGUIState& gui) const;
  bool LoadProcStateFromVar(juce::var const& json, FBProcStateContainer& proc) const;
  bool LoadEditStateFromVar(juce::var const& json, FBScalarStateContainer& edit) const;

  bool LoadGUIStateFromString(std::string const& text, FBGUIState& gui) const;
  bool LoadProcStateFromString(std::string const& text, FBProcStateContainer& proc) const;
  bool LoadEditStateFromString(std::string const& text, FBScalarStateContainer& edit) const;

  bool LoadGUIStateFromStringWithDryRun(std::string const& text, FBGUIState& gui) const;
  bool LoadProcStateFromStringWithDryRun(std::string const& text, FBProcStateContainer& proc) const;
  bool LoadEditStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& edit) const;
};