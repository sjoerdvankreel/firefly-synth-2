#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBSteppedParamNonRealTime.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct FBItemsParamNonRealTime:
public FBSteppedParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBItemsParamNonRealTime);
  virtual juce::PopupMenu MakePopupMenu() const = 0;
  bool IsItems() const override final { return true; }
};