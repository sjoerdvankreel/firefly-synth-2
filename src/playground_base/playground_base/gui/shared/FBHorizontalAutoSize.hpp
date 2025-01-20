#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class IFBHorizontalAutoSize
{
public:
  virtual int FixedWidth() const = 0;
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBHorizontalAutoSize);
};