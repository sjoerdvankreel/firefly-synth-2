#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

struct FBStaticTopo;
struct FBTopoIndices;
struct FBRuntimeTopo;
struct FBParamTopoIndices;

class FBGUIStateContainer;
class FBExchangeStateContainer;

struct FBHostContextMenuItem
{
  int hostTag = -1;
  std::string name = {};
  bool checked = false;
  bool enabled = false;
  bool separator = false;
  bool subMenuEnd = false;
  bool subMenuStart = false;
};

void
FBAddHostContextMenu(
  std::shared_ptr<juce::PopupMenu> menu,
  int offset,
  std::vector<FBHostContextMenuItem> const& items);

class IFBHostGUIContextListener
{
public:
  virtual ~IFBHostGUIContextListener() {}
  virtual void OnPatchLoaded() = 0;
  virtual void OnGUIAudioClipsChanged() = 0;
  virtual void OnPatchNameChanged(std::string const& name) = 0;
  virtual void OnInstanceNameChanged(std::string const& name) = 0;
};

class FBHostGUIContext
{
  std::vector<IFBHostGUIContextListener*> _listeners = {};

protected:
  std::unique_ptr<FBRuntimeTopo> _topo;
  std::unique_ptr<FBGUIStateContainer> _guiState;
  std::unique_ptr<FBExchangeStateContainer> _exchangeFromDSPState;

private:
  bool _isPatchLoaded = {};
  FBUndoStateContainer _undoState;

  // copy of patch/session state on daw load/patch load
  // we use it to be able to highlight params tweaked from patch/session
  // used to be able to revert back to it, but since introduction of sampler, no longer possible
  FBScalarStateContainer _patchState;
  FBScalarStateContainer _sessionState;
  
  // gui side clips
  std::map<std::string, FBAudioClip> _guiAudioClips = {};

  void ShowOnlineManualForModule(int index) const;

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostGUIContext);
  FBHostGUIContext(std::unique_ptr<FBStaticTopo>&& topo);

  void OnPatchLoaded();
  void OnPatchNameChanged();
  void OnInstanceNameChanged();
  void OnGUIAudioClipsChanged();

  virtual void DoEndAudioParamChange(int index) = 0;
  virtual void DoBeginAudioParamChange(int index) = 0;
  virtual void DoPerformAudioParamEdit(int index, double normalized) = 0;

public:
  FBAudioClip const* FindGUIAudioClip(std::string const& id) const;
  void SetGUIAudioClips(std::map<std::string, FBAudioClip> const& clips);
  std::map<std::string, FBAudioClip>& GetGUIAudioClips() { return _guiAudioClips; }
  std::map<std::string, FBAudioClip> const& GetGUIAudioClips() const { return _guiAudioClips; }

  void ClearGUIAndDSPAudioClips();
  void RemoveGUIAndDSPAudioClip(std::string const& id);
  void SetGUIAndDSPAudioClip(std::string const& id, FBAudioClip const& guiClip, FBAudioClip const& dspClip);

  virtual void ClearDSPAudioClipToDSP(std::string const& id) = 0;
  virtual void PushDSPAudioClipToDSP(std::string const& id, FBAudioClip const& clip) = 0;
  virtual std::map<std::string, FBAudioClip> PullDSPAudioClipsToGUI() const = 0;
  virtual void PushDSPAudioClipsToDSP(std::map<std::string, FBAudioClip> const& dspClips) const = 0;

  void MarkPatchAsSessionState();
  void MarkPatchAsPatchState(std::string const& name);

  double GetUserScaleMin() const;
  double GetUserScaleMax() const;
  double GetUserScalePlain() const;
  void SetUserScalePlain(double scale);

  std::string const& PatchName() const;
  std::string const& ThemeName() const;
  std::string const& InstanceName() const;
  void SetPatchName(std::string const& name);
  void SetThemeName(std::string const& name);
  void SetInstanceName(std::string const& name);
  
  bool IsPatchLoaded() const { return _isPatchLoaded; }
  
  void ShowOnlineManual() const;
  void ShowOnlineManualForGUIParam(int index) const;
  void ShowOnlineManualForAudioParam(int index) const;
  virtual std::string OnlineManualLocation() const = 0;

  void AddListener(IFBHostGUIContextListener* listener);
  void RemoveListener(IFBHostGUIContextListener* listener);

  bool GetGUIParamBool(FBParamTopoIndices const& indices) const;
  int GetGUIParamDiscrete(FBParamTopoIndices const& indices) const;
  template <class T> T GetGUIParamList(FBParamTopoIndices const& indices) const;

  void SetGUIParamList(FBParamTopoIndices const& indices, int val);
  void SetGUIParamBool(FBParamTopoIndices const& indices, bool val);
  void SetGUIParamDiscrete(FBParamTopoIndices const& indices, int val);

  bool GetAudioParamBool(FBParamTopoIndices const& indices) const;
  void SetAudioParamBool(FBParamTopoIndices const& indices, bool value);

  int GetAudioParamDiscrete(FBParamTopoIndices const& indices) const;
  void SetAudioParamDiscrete(FBParamTopoIndices const& indices, int value);

  std::string GetAudioParamText(FBParamTopoIndices const& indices) const;
  double GetAudioParamLinear(FBParamTopoIndices const& indices) const;
  double GetAudioParamIdentity(FBParamTopoIndices const& indices) const;
  double GetAudioParamNormalized(FBParamTopoIndices const& indices) const;
  template <class T> T GetAudioParamList(FBParamTopoIndices const& indices) const;

  void EndAudioParamChange(int index);
  void BeginAudioParamChange(int index);
  void PerformAudioParamEdit(int index, double normalized);

  void DefaultAudioParam(FBParamTopoIndices const& indices);
  void ClearModuleAudioParams(FBTopoIndices const& moduleIndices);
  void CopyModuleAudioParams(FBTopoIndices const& moduleIndices, int toSlot);
  void CopyAudioParam(FBParamTopoIndices const& from, FBParamTopoIndices const& to);
  void PerformImmediateAudioParamEdit(int index, double normalized);
  void PerformImmediateAudioParamEdit(FBParamTopoIndices const& indices, double normalized);

  double GetGUIParamNormalized(int index) const;
  void SetGUIParamNormalized(int index, double normalized);
  FBUndoStateContainer& UndoState() { return _undoState; }
  FBRuntimeTopo const* Topo() const { return _topo.get(); }
  FBGUIStateContainer* GUIState() { return _guiState.get(); }
  FBGUIStateContainer const* GUIState() const { return _guiState.get(); }
  FBExchangeStateContainer const* ExchangeFromDSPState() const { return _exchangeFromDSPState.get(); }

  FBScalarStateContainer const& PatchState() const { return _patchState; }
  FBScalarStateContainer const& SessionState() const { return _sessionState; }

  virtual void NotifyHostOfParamNameChanges() = 0;
  virtual void ClearAudioParamNameOverrides() = 0;
  virtual void ClearAudioParamNameOverride(int index) = 0;
  virtual bool GetAudioParamNameOverride(int index, std::string& name) const = 0;
  virtual void SetAudioParamNameOverride(int index, std::string const& name) = 0;
  std::map<int, std::string> GetAudioParamNameOverrides() const;
  void SetAudioParamNameOverrides(std::map<int, std::string> const& overrides);

  virtual double GetAudioParamNormalized(int index) const = 0;
  virtual void AudioParamContextMenuClicked(int paramIndex, int juceTag) = 0;
  virtual std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) = 0;
};

template <class T>
T FBHostGUIContext::GetGUIParamList(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return static_cast<T>(param->static_.List().NormalizedToPlainFast(static_cast<float>(normalized)));
}

template <class T>
T FBHostGUIContext::GetAudioParamList(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return static_cast<T>(param->static_.List().NormalizedToPlainFast(static_cast<float>(normalized)));
}