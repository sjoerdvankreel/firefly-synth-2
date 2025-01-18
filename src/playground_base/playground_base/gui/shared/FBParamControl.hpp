#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class IFBParamControl
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBParamControl);
  virtual void SetValueNormalized(float normalized) = 0;
};