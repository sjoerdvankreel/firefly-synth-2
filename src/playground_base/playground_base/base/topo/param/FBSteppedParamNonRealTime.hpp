#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

struct FBSteppedParamNonRealTime:
public FBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBSteppedParamNonRealTime);
  bool IsStepped() const override final { return true; }
};