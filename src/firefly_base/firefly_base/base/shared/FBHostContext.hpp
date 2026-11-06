#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/shared/FBExchangeStateQueue.hpp>
#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <readerwriterqueue.h>

#include <map>
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <filesystem>

struct FBStaticTopo;
struct FBTopoIndices;
struct FBRuntimeTopo;
struct FBParamTopoIndices;

class MTSClient;
class IFBPlugProcessor;
class FBGUIStateContainer;
class FBExchangeStateContainer;

// DSP audio clips are owned by the GUI thread, and borrowed by the audio thread.
// GUI sends non-owning pointers to the audio thread, audio thread sends them
// back when it no longer uses them, GUI deletes them only after that.
struct FBAudioClipToDSPCommand
{
  enum { ClearSingle, LoadSingle, LoadAll } Type = {};
  std::string id = {};
  FBAudioClip const* clip = {};
  std::vector<std::pair<std::string, FBAudioClip const*>> allClips = {};
};

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

class IFBHostContextGUIListener
{
public:
  virtual ~IFBHostContextGUIListener() {}
  virtual void OnPatchLoaded() = 0;
  virtual void OnGUIAudioClipsChanged() = 0;
  virtual void OnPatchNameChanged(std::string const& name) = 0;
  virtual void OnInstanceNameChanged(std::string const& name) = 0;
};

void
FBAddHostContextMenu(
  std::shared_ptr<juce::PopupMenu> menu,
  int offset,
  std::vector<FBHostContextMenuItem> const& items);

class FBHostContext:
public juce::Timer
{
  bool _isPatchLoaded = {};
  std::vector<IFBHostContextGUIListener*> _guiListeners = {};

  // GUI thread only. Small, downsampled clips for display and graphs.
  // The views point into _guiAudioClips, for graph rendering.
  std::map<std::string, FBAudioClip> _guiAudioClips = {};
  std::map<std::string, FBAudioClip const*> _guiAudioClipViews = {};

  // GUI thread only. Owner of all full-size DSP clips.
  // Retired clips are no longer current but may still be in use
  // by the audio thread, until it sends them back.
  std::map<std::string, std::unique_ptr<FBAudioClip>> _ownedDSPAudioClips = {};
  std::map<FBAudioClip const*, std::unique_ptr<FBAudioClip>> _retiredDSPAudioClips = {};

  // Audio thread only. Non-owning, borrowed from _ownedDSPAudioClips.
  std::map<std::string, FBAudioClip const*> _dspAudioClips = {};

  std::unique_ptr<FBRuntimeTopo> _topo;
  MTSClient* _mtsClient;

  FBUndoStateContainer _undoState;
  // copy of patch/session state on daw load/patch load
  // we use it to be able to highlight params tweaked from patch/session
  // used to be able to revert back to it, but since introduction of sampler, no longer possible
  FBScalarStateContainer _patchState;
  FBScalarStateContainer _sessionState;
  std::unique_ptr<FBGUIStateContainer> _guiState;
  std::unique_ptr<FBProcStateContainer> _procState;
  std::unique_ptr<FBExchangeStateContainer> _exchangeToGUIState;
  std::unique_ptr<FBExchangeStateContainer> _exchangeFromDSPState;
  std::unique_ptr<FBExchangeStateQueueBase> _exchangeStateQueue;

  // Both unbounded. Enqueueing may allocate, but only happens as
  // a result of loading clips, never during plain processing.
  moodycamel::ReaderWriterQueue<FBAudioClipToDSPCommand, 16> _audioClipToDSPCommands;
  moodycamel::ReaderWriterQueue<FBAudioClip const*, 16> _audioClipsReturnedFromDSP;
  moodycamel::ReaderWriterQueue<std::vector<float>, 16> _audioParamsToDSPQueue;

  void ShowOnlineManualForModule(int index) const;
  void RebuildGUIAudioClipViews();
  void RetireOwnedDSPAudioClip(std::string const& id);
  void ReleaseDSPAudioClipsReturnedFromDSP();

protected:
  FBHostContext(
    std::unique_ptr<FBStaticTopo>&& topo,
    MTSClient* mtsClient,
    std::unique_ptr<FBExchangeStateQueueBase>&& exchangeStateQueue);

  void OnPatchLoaded();
  void OnPatchNameChanged();
  void OnInstanceNameChanged();
  void OnGUIAudioClipsChanged();

  virtual void OnReceivedExchangeFromDSP() = 0;
  virtual void DoEndAudioParamChange(int index) = 0;
  virtual void DoBeginAudioParamChange(int index) = 0;
  virtual void DoPerformAudioParamEdit(int index, double normalized) = 0;

  FBExchangeStateQueueBase* ExchangeStateQueue() { return _exchangeStateQueue.get(); }

public:
  virtual ~FBHostContext();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostContext);

  // pull exchange from dsp to gui
  void timerCallback() override;
  MTSClient* GetMTSClient() { return _mtsClient; }

  virtual float SampleRate() const = 0;
  virtual std::unique_ptr<IFBPlugProcessor> MakePlugProcessor() = 0;

  virtual void NotifyHostOfParamNameChangesOnly() = 0;
  virtual void ClearAudioParamNameOverrides() = 0;
  virtual void ClearAudioParamNameOverride(int index) = 0;
  virtual bool GetAudioParamNameOverride(int index, std::string& name) const = 0;
  virtual void SetAudioParamNameOverride(int index, std::string const& name) = 0;

  virtual void NotifyHostOfBulkUpdateAndParamNameChanges() = 0;
  virtual void BulkUpdateAudioParamsToGUI(FBScalarStateContainer const& audioParams, bool patchOnly) = 0;

  virtual double GetAudioParamNormalized(int index) const = 0;
  virtual void AudioParamContextMenuClicked(int paramIndex, int juceTag) = 0;
  virtual std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) = 0;

  void PushProcStateToDSP();
  void PullProcStateFromGUI();

  // Audio thread.
  void PullDSPAudioClipsFromGUI();
  std::map<std::string, FBAudioClip const*> const& GetDSPAudioClips() const { return _dspAudioClips; }

  // GUI thread. None of these wait for the audio thread.
  void ClearDSPAudioClipToDSP(std::string const& id);
  void PushDSPAudioClipToDSP(std::string const& id, FBAudioClip const& clip);
  void PushDSPAudioClipsToDSP(std::map<std::string, FBAudioClip> const& dspClips);
  std::map<std::string, FBAudioClip> PullDSPAudioClipsToGUI() const;
  bool PullDSPAudioClipToGUI(std::string const& id, FBAudioClip& clip) const;

  void ClearGUIAndDSPAudioClips();
  void RemoveGUIAndDSPAudioClip(std::string const& id);
  void SetGUIAndDSPAudioClip(std::string const& id, FBAudioClip const& guiClip, FBAudioClip const& dspClip);

  FBProcStateContainer* ProcState() { return _procState.get(); }
  FBExchangeStateContainer* ExchangeToGUIState() { return _exchangeToGUIState.get(); }

  FBAudioClip const* FindGUIAudioClip(std::string const& id) const;
  void SetGUIAudioClips(std::map<std::string, FBAudioClip> const& clips);
  std::map<std::string, FBAudioClip> const& GetGUIAudioClips() const { return _guiAudioClips; }
  std::map<std::string, FBAudioClip const*> const& GetGUIAudioClipViews() const { return _guiAudioClipViews; }

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
  std::map<int, std::string> GetAudioParamNameOverrides() const;
  void SetAudioParamNameOverrides(std::map<int, std::string> const& overrides);
  FBScalarStateContainer const& PatchState() const { return _patchState; }
  FBScalarStateContainer const& SessionState() const { return _sessionState; }

  void ShowOnlineManual() const;
  void ShowOnlineManualForGUIParam(int index) const;
  void ShowOnlineManualForAudioParam(int index) const;
  virtual std::string OnlineManualLocation() const = 0;

  void AddGUIListener(IFBHostContextGUIListener* listener);
  void RemoveGUIListener(IFBHostContextGUIListener* listener);

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
  FBExchangeStateContainer* ExchangeFromDSPState() { return _exchangeFromDSPState.get(); }
  FBExchangeStateContainer const* ExchangeFromDSPState() const { return _exchangeFromDSPState.get(); }
};

template <class T>
T FBHostContext::GetGUIParamList(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->gui.ParamAtTopo(indices);
  double normalized = GetGUIParamNormalized(param->runtimeParamIndex);
  return static_cast<T>(param->static_.List().NormalizedToPlainFast(static_cast<float>(normalized)));
}

template <class T>
T FBHostContext::GetAudioParamList(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return static_cast<T>(param->static_.List().NormalizedToPlainFast(static_cast<float>(normalized)));
}