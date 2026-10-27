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
struct FBPlugVersion;

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
  bool LoadParamStateFromVar(
    juce::var const& json, FBPlugVersion const& loadingVersion, 
    bool isGuiState, TContainer& container, TParamsTopo& params, bool patchOnly) const;

  juce::var SaveAudioClipsToVar(std::map<std::string, FBAudioClip> const& audioClips) const;
  juce::var SaveParamNameOverridesToVar(std::map<int, std::string> const& paramNameOverrides) const;
  juce::var SaveStateToVar(FBHostContext const& hostContext, FBScalarStateContainer const& audioParams, bool patchOnly) const;

  bool LoadAudioClipsFromVar(juce::var const& json, std::map<std::string, FBAudioClip>& audioClips) const;
  bool LoadParamNameOverridesFromVar(juce::var const& json, std::map<int, std::string>& paramNameOverrides) const;
  bool LoadStateFromVar(juce::var const& json, FBHostContext& hostContext, FBScalarStateContainer& audioParams, bool patchOnly) const;
  bool LoadStateFromBytes(std::vector<std::uint8_t> const& bytes, FBHostContext& hostContext, FBScalarStateContainer& audioParams, bool patchOnly) const;

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
  std::vector<std::uint8_t> SaveStateToBytes(FBHostContext const& hostContext, FBScalarStateContainer const& audioParams, bool patchOnly) const;

  bool LoadVST3PreFormat2GUIFromBytes(std::vector<std::uint8_t> const& bytes, FBHostContext& hostContext) const;
  bool LoadVST3FromBytesWithPreFormat2Check(std::vector<std::uint8_t> const& bytes, FBHostContext& hostContext, FBScalarStateContainer& audioParams) const;
  bool LoadCLAPFromBytesWithPreFormat2Check(std::vector<std::uint8_t> const& bytes, FBHostContext& hostContext, FBScalarStateContainer& audioParams) const;
  bool LoadPatchFromBytesWithPreFormat2Check(std::vector<std::uint8_t> const& bytes, FBHostContext& hostContext, FBScalarStateContainer& audioParams) const;
};