#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class IFBHostGUIContext
{
public:
  FB_NOCOPY_MOVE_DEFCTOR(IFBHostGUIContext);
  virtual void SetParamNormalized(int index, float normalized) = 0;
  virtual void GetParamNormalized(int index, float normalized) const = 0;
};