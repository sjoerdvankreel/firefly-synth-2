#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class IFBHostGUIContext
{
public:
  FB_NOCOPY_MOVE_DEFCTOR(IFBHostGUIContext);

  virtual void EndParamChange(int index) = 0;
  virtual void BeginParamChange(int index) = 0;
  virtual void PerformParamEdit(int index, float normalized) = 0;

  virtual float GetParamNormalized(int index) const = 0;
  virtual std::unique_ptr<juce::PopupMenu> ParamContextMenu(int index) = 0;
};