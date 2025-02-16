#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBStaticTopo.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeModule.hpp>
#include <playground_base/base/topo/runtime/FBParamTopoIndices.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParamsTopo.hpp>

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

  bool LoadGUIStateFromStringWithDryRun(std::string const& text, FBGUIStateContainer& gui) const;
  bool LoadProcStateFromStringWithDryRun(std::string const& text, FBProcStateContainer& proc) const;
  bool LoadEditStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& edit) const;
  bool LoadEditAndGUIStateFromStringWithDryRun(std::string const& text, FBScalarStateContainer& edit, FBGUIStateContainer& gui) const;
};