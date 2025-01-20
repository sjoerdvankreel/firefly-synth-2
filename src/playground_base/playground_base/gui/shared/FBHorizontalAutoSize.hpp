#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class IFBHorizontalAutoSize
{
public:
  virtual int FixedWidth(int height) const = 0;
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBHorizontalAutoSize);
};

IFBHorizontalAutoSize*
FBAsHorizontalAutoSize(juce::Component* component);