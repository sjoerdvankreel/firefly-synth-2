#pragma once

#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBItemsParamNonRealTime:
public FBParamNonRealTime
{
  virtual juce::PopupMenu MakePopupMenu() const = 0;
};