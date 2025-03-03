#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBParamNonRealTime.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

struct IFBListParamNonRealTime:
public IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBListParamNonRealTime); 
  virtual juce::PopupMenu MakePopupMenu() const = 0;
};