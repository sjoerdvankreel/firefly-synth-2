#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/gui/glue/FBHostContextMenu.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

struct FBRuntimeTopo;
class FBGUIStateContainer;
class FBExchangeStateContainer;

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