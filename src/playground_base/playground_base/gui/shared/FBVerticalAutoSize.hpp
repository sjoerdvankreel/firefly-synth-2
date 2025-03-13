#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class IFBVerticalAutoSize
{
public:
  virtual int FixedHeight() const = 0;
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBVerticalAutoSize);
};