#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/main/FBUndoStateContainer.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <string>
#include <vector>

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
  FBHostGUIContext();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostGUIContext);

  virtual void DoEndAudioParamChange(int index) = 0;
  virtual void DoBeginAudioParamChange(int index) = 0;
  virtual void DoPerformAudioParamEdit(int index, double normalized) = 0;

public:
  virtual FBGUIStateContainer* GUIState() = 0;
  virtual FBRuntimeTopo const* Topo() const = 0;
  virtual FBExchangeStateContainer const* ExchangeState() const = 0;

  virtual double GetAudioParamNormalized(int index) const = 0;
  virtual double GetGUIParamNormalized(int index) const = 0;
  virtual void SetGUIParamNormalized(int index, double normalized) = 0;

  virtual void AudioParamContextMenuClicked(int paramIndex, int juceTag) = 0;
  virtual std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) = 0;

  FBUndoStateContainer& UndoState() { return _undoState; }
  int GetGUIParamDiscrete(FBParamTopoIndices const& indices) const;

  void EndAudioParamChange(int index);
  void BeginAudioParamChange(int index);
  void PerformAudioParamEdit(int index, double normalized);

  void PerformImmediateAudioParamEdit(int index, double normalized);
  void ClearModuleAudioParams(FBTopoIndices const& moduleIndices);
  void CopyModuleAudioParams(FBTopoIndices const& moduleIndices, int toSlot);
};