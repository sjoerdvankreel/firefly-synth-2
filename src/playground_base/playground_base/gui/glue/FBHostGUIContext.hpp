#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <string>
#include <vector>

struct FBRuntimeTopo;
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
public:
  FB_NOCOPY_MOVE_DEFCTOR(FBHostGUIContext);

  virtual FBGUIStateContainer* GUIState() = 0;
  virtual FBRuntimeTopo const* Topo() const = 0;
  virtual FBExchangeStateContainer const* ExchangeState() const = 0;

  virtual void EndAudioParamChange(int index) = 0;
  virtual void BeginAudioParamChange(int index) = 0;
  virtual void PerformAudioParamEdit(int index, double normalized) = 0;
  void PerformImmediateAudioParamEdit(int index, double normalized);

  virtual double GetAudioParamNormalized(int index) const = 0;
  virtual double GetGUIParamNormalized(int index) const = 0;
  virtual void SetGUIParamNormalized(int index, double normalized) = 0;

  virtual void AudioParamContextMenuClicked(int paramIndex, int juceTag) = 0;
  virtual std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) = 0;
};