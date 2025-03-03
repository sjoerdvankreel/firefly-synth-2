#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBParamNonRealTime);
  virtual bool IsItems() const = 0;
  virtual bool IsStepped() const = 0;
  virtual int ValueCount() const = 0;
};