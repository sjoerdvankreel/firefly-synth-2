#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/glue/FBHostContextMenu.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

struct FBGUIState;
struct FBRuntimeTopo;
class FBExchangeStateContainer;

class FBHostGUIContext
{
public:
  FB_NOCOPY_MOVE_DEFCTOR(FBHostGUIContext);

  virtual void EndParamChange(int index) = 0;
  virtual void BeginParamChange(int index) = 0;
  void PerformImmediateParamEdit(int index, float normalized);
  virtual void PerformParamEdit(int index, float normalized) = 0;

  virtual FBGUIState* GUIState() = 0;
  virtual FBRuntimeTopo const* Topo() const = 0;
  virtual FBExchangeStateContainer const* ExchangeState() const = 0;

  virtual float GetParamNormalized(int index) const = 0;
  virtual void ParamContextMenuClicked(int paramIndex, int juceTag) = 0;
  virtual std::vector<FBHostContextMenuItem> MakeParamContextMenu(int index) = 0;
};