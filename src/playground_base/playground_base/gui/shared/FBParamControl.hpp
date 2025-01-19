#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBRuntimeParam;

class FBParamControl
{
protected:
  FBRuntimeParam const* const _param;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamControl);
  FBParamControl(FBRuntimeParam const* param);
  
  virtual void SetValueNormalized(float normalized) = 0;
  FBRuntimeParam const* Param() const { return _param; }
};