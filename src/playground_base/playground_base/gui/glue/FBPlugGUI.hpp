#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class IFBParamControl;

class FBPlugGUI:
public juce::Component
{
protected:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugGUI);

public:
  virtual IFBParamControl*
  GetParamControlForIndex(int index) = 0;
};