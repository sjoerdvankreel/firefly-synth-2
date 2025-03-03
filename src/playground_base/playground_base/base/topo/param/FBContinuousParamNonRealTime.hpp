#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <string>
#include <optional>

struct FBContinuousParamNonRealTime:
public FBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBContinuousParamNonRealTime);
  int ValueCount() const override final { return 0; }
  bool IsItems() const override final { return false; }
  bool IsStepped() const override final { return false; }
};