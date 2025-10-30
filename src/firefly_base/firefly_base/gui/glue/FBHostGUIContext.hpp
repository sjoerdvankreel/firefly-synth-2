#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <string>
#include <vector>

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

std::unique_ptr<juce::PopupMenu>
FBMakeHostContextMenu(
  std::vector<FBHostContextMenuItem> const& items);

class FBHostGUIContext
{
  FBUndoStateContainer _undoState;

protected:
  std::unique_ptr<FBRuntimeTopo> _topo;
  std::unique_ptr<FBGUIStateContainer> _guiState;
  std::unique_ptr<FBExchangeStateContainer> _exchangeFromDSPState;

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostGUIContext);
  FBHostGUIContext(std::unique_ptr<FBStaticTopo>&& topo);

  virtual void DoEndAudioParamChange(int index) = 0;
  virtual void DoBeginAudioParamChange(int index) = 0;
  virtual void DoPerformAudioParamEdit(int index, double normalized) = 0;

public:
  virtual double GetAudioParamNormalized(int index) const = 0;
  virtual void AudioParamContextMenuClicked(int paramIndex, int juceTag) = 0;
  virtual std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) = 0;

  double GetUserScaleMin() const;
  double GetUserScaleMax() const;
  double GetUserScalePlain() const;
  void SetUserScalePlain(double scale);

  bool GetGUIParamBool(FBParamTopoIndices const& indices) const;
  int GetGUIParamDiscrete(FBParamTopoIndices const& indices) const;

  bool GetAudioParamBool(FBParamTopoIndices const& indices) const;
  int GetAudioParamDiscrete(FBParamTopoIndices const& indices) const;
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
  FBExchangeStateContainer const* ExchangeFromDSPState() const { return _exchangeFromDSPState.get(); }
};

template <class T>
T FBHostGUIContext::GetAudioParamList(FBParamTopoIndices const& indices) const
{
  auto param = Topo()->audio.ParamAtTopo(indices);
  double normalized = GetAudioParamNormalized(param->runtimeParamIndex);
  return static_cast<T>(param->static_.List().NormalizedToPlainFast(static_cast<float>(normalized)));
}